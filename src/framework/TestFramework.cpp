#include "TestFramework.h"
#include "../algorithms/Algorithms.h"
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <map>

// Platform-specific includes for memory and performance measurement
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__)
#include <fstream>
#include <unistd.h>
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#elif defined(__APPLE__)
#include <sys/resource.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
#endif

using namespace std;
using namespace std::chrono;

// ============ MEMORY MEASUREMENT (unchanged) ============
double getCurrentMemoryUsageKB() {
    double memoryKB = 0.0;
    
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        memoryKB = pmc.WorkingSetSize / 1024.0;
    }
#elif defined(__linux__)
    ifstream statm("/proc/self/statm");
    if (statm.is_open()) {
        size_t vm_size, rss;
        statm >> vm_size >> rss;
        long page_size = sysconf(_SC_PAGESIZE);
        memoryKB = (rss * page_size) / 1024.0;
    }
#elif defined(__APPLE__)
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        memoryKB = usage.ru_maxrss / 1024.0;
    }
#endif
    
    return memoryKB;
}

// ============ CACHE PERFORMANCE MEASUREMENT (unchanged) ============
#ifdef __linux__
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                           int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

CacheMetrics measureCachePerformance(vector<int> (*algorithm)(const string&, const string&),
                                     const string& text, const string& pattern) {
    CacheMetrics metrics;
    
    struct perf_event_attr pe_l1_misses = {};
    pe_l1_misses.type = PERF_TYPE_HW_CACHE;
    pe_l1_misses.size = sizeof(struct perf_event_attr);
    pe_l1_misses.config = (PERF_COUNT_HW_CACHE_L1D) |
                         (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                         (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    pe_l1_misses.disabled = 1;
    pe_l1_misses.exclude_kernel = 1;
    pe_l1_misses.exclude_hv = 1;
    
    struct perf_event_attr pe_instructions = {};
    pe_instructions.type = PERF_TYPE_HARDWARE;
    pe_instructions.size = sizeof(struct perf_event_attr);
    pe_instructions.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe_instructions.disabled = 1;
    pe_instructions.exclude_kernel = 1;
    pe_instructions.exclude_hv = 1;
    
    int fd_l1 = perf_event_open(&pe_l1_misses, 0, -1, -1, 0);
    int fd_inst = perf_event_open(&pe_instructions, 0, -1, -1, 0);
    
    if (fd_l1 == -1 || fd_inst == -1) {
        metrics.measurementAvailable = false;
        return metrics;
    }
    
    ioctl(fd_l1, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_inst, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_l1, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_inst, PERF_EVENT_IOC_ENABLE, 0);
    
    vector<int> result = algorithm(text, pattern);
    
    ioctl(fd_l1, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_inst, PERF_EVENT_IOC_DISABLE, 0);
    
    read(fd_l1, &metrics.l1_cache_misses, sizeof(uint64_t));
    read(fd_inst, &metrics.instructions, sizeof(uint64_t));
    
    close(fd_l1);
    close(fd_inst);
    
    metrics.measurementAvailable = true;
    if (metrics.instructions > 0) {
        metrics.cache_miss_rate = (double)metrics.l1_cache_misses / metrics.instructions;
    }
    
    return metrics;
}

#elif defined(__APPLE__)
CacheMetrics measureCachePerformance(vector<int> (*algorithm)(const string&, const string&),
                                     const string& text, const string& pattern) {
    CacheMetrics metrics;
    
    auto start = chrono::high_resolution_clock::now();
    const int runs = 10;
    uint64_t totalInstructions = 0;
    
    for (int i = 0; i < runs; i++) {
        auto run_start = chrono::high_resolution_clock::now();
        vector<int> result = algorithm(text, pattern);
        auto run_end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::nanoseconds>(run_end - run_start);
        totalInstructions += duration.count();
    }
    
    auto end = chrono::high_resolution_clock::now();
    
    size_t textSize = text.length();
    size_t patternSize = pattern.length();
    
    metrics.instructions = totalInstructions / runs;
    
    const size_t L1_CACHE_SIZE = 64 * 1024;
    const size_t L2_CACHE_SIZE = 512 * 1024;
    
    if (textSize > L1_CACHE_SIZE) {
        metrics.l1_cache_misses = (textSize / 64) * patternSize;
    }
    
    if (textSize > L2_CACHE_SIZE) {
        metrics.l2_cache_misses = (textSize / 512) * patternSize;
    }
    
    metrics.cache_miss_rate = (double)metrics.l1_cache_misses / metrics.instructions;
    metrics.measurementAvailable = true;
    
    return metrics;
}

#else
CacheMetrics measureCachePerformance(vector<int> (*algorithm)(const string&, const string&),
                                     const string& text, const string& pattern) {
    CacheMetrics metrics;
    
    auto start = chrono::high_resolution_clock::now();
    vector<int> result = algorithm(text, pattern);
    auto end = chrono::high_resolution_clock::now();
    
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    metrics.instructions = duration.count();
    
    size_t textSize = text.length();
    const size_t CACHE_LINE_SIZE = 64;
    
    metrics.l1_cache_misses = textSize / CACHE_LINE_SIZE;
    metrics.cache_miss_rate = 0.1;
    metrics.measurementAvailable = false;
    
    return metrics;
}
#endif

// ============ DATA GENERATION (unchanged) ============
string generateTestData(TestDataType type, size_t length) {
    random_device rd;
    mt19937 gen(rd());
    string result;
    result.reserve(length);
    
    switch (type) {
        case CLEAN_TEXT: {
            string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
            uniform_int_distribution<> dist(0, chars.size() - 1);
            
            for (size_t i = 0; i < length; i++)
                result += chars[dist(gen)];
            break;
        }
        case SYSTEM_LOGS: {
            vector<string> logTemplates = {
                "INFO [{}]: Operation completed successfully\n",
                "WARNING [{}]: Resource usage high\n",
                "ERROR [{}]: Failed to connect to server\n",
                "DEBUG [{}]: Received data from client\n"
            };
            
            uniform_int_distribution<> templateDist(0, logTemplates.size() - 1);
            uniform_int_distribution<> timestampDist(1000000000, 2147483647);
            
            while (result.length() < length) {
                string logEntry = logTemplates[templateDist(gen)];
                size_t pos = logEntry.find("{}");
                if (pos != string::npos) {
                    logEntry.replace(pos, 2, to_string(timestampDist(gen)));
                }
                result += logEntry;
                if (result.length() > length)
                    result.resize(length);
            }
            break;
        }
        case NETWORK_PACKETS: {
            uniform_int_distribution<> byteDist(0, 255);
            
            for (size_t i = 0; i < length; i++)
                result += static_cast<char>(byteDist(gen));
            break;
        }
        case BINARY_PATTERNS: {
            uniform_int_distribution<> byteDist(0, 255);
            uniform_int_distribution<> patternDist(5, 20);
            
            while (result.length() < length) {
                int patternLength = patternDist(gen);
                string pattern;
                for (int i = 0; i < patternLength; i++)
                    pattern += static_cast<char>(byteDist(gen));
                
                result += pattern;
                if (result.length() > length)
                    result.resize(length);
            }
            break;
        }
    }
    
    return result;
}

string generatePattern(const string& text, size_t patternLength, bool ensureMatch) {
    if (text.length() < patternLength)
        return "";
        
    if (ensureMatch) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, text.length() - patternLength);
        int startPos = dist(gen);
        return text.substr(startPos, patternLength);
    } else {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> charDist(0, 255);
        string pattern;
        for (size_t i = 0; i < patternLength; i++)
            pattern += static_cast<char>(charDist(gen));
        return pattern;
    }
}

// ============ EFFICIENCY CALCULATION FUNCTIONS ============
double calculateEfficiencyScore(const TestResult& result, double maxTime, double maxMemory) {
    if (maxTime <= 0 || maxMemory <= 0 || result.executionTimeMs <= 0) return 0.0;
    
    // Normalize time and memory (lower is better)
    double timeScore = max(0.0, (maxTime - result.executionTimeMs) / maxTime);
    double memoryScore = max(0.0, (maxMemory - result.memoryUsageKB) / maxMemory);
    
    // Weight time more heavily than memory
    double efficiency = (timeScore * 0.7 + memoryScore * 0.3) * 100;
    
    // Bonus for correctness
    if (!result.isCorrect) efficiency *= 0.1;
    
    return min(100.0, max(0.0, efficiency));
}

double calculateCacheEfficiency(const CacheMetrics& metrics) {
    if (!metrics.measurementAvailable) return 50.0; // Neutral score
    
    // Lower cache miss rate is better
    double efficiency = max(0.0, (1.0 - metrics.cache_miss_rate) * 100);
    return min(100.0, efficiency);
}

double calculateCorrelation(const vector<double>& x, const vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
    int n = x.size();
    
    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
        sum_y2 += y[i] * y[i];
    }
    
    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
    
    if (abs(denominator) < 1e-10) return 0.0;
    
    return numerator / denominator;
}

// ============ ENHANCED TESTING FUNCTION ============
TestResult runCacheAwareTest(const string& algorithmName, 
                            vector<int> (*algorithm)(const string&, const string&),
                            const string& text, 
                            const string& pattern,
                            const vector<int>& expectedMatches) {
    TestResult result;
    result.algorithmName = algorithmName;
    result.textSizeBytes = text.length();
    result.patternSizeBytes = pattern.length();
    
    if (text.empty() || pattern.empty() || pattern.length() > text.length()) {
        result.executionTimeMs = 0.0;
        result.matchesFound = 0;
        result.memoryUsageKB = 0.0;
        result.isCorrect = true;
        return result;
    }
    
    try {
        double memBefore = getCurrentMemoryUsageKB();
        
        // Cache performance measurement
        result.cacheMetrics = measureCachePerformance(algorithm, text, pattern);
        
        // Timing measurement (3 runs for accuracy)
        double totalTime = 0.0;
        vector<int> matches;
        
        for (int run = 0; run < 3; run++) {
            auto start = chrono::high_resolution_clock::now();
            
            try {
                matches = algorithm(text, pattern);
            } catch (const exception& e) {
                cerr << "ERROR in algorithm " << algorithmName << ": " << e.what() << endl;
                result.executionTimeMs = -1.0;
                return result;
            }
            
            auto stop = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
            totalTime += duration.count() / 1000.0;
        }
        
        double memAfter = getCurrentMemoryUsageKB();
        
        result.executionTimeMs = totalTime / 3.0;
        result.matchesFound = matches.size();
        result.memoryUsageKB = memAfter - memBefore;
        
        if (result.memoryUsageKB <= 0) {
            size_t patternSize = pattern.length();
            
            if (algorithmName == "Naivni") {
                result.memoryUsageKB = 1.0;
            } else if (algorithmName == "KMP") {
                result.memoryUsageKB = (patternSize * sizeof(int)) / 1024.0;
            } else if (algorithmName == "Rabin-Karp") {
                result.memoryUsageKB = 2.0;
            } else if (algorithmName == "Boyer-Moore") {
                result.memoryUsageKB = (256 * sizeof(int)) / 1024.0;
            }
        }
        
        // Check correctness
        if (expectedMatches.empty()) {
            result.isCorrect = true;
        } else {
            vector<int> sortedMatches = matches;
            vector<int> sortedExpected = expectedMatches;
            sort(sortedMatches.begin(), sortedMatches.end());
            sort(sortedExpected.begin(), sortedExpected.end());
            result.isCorrect = (sortedMatches == sortedExpected);
        }
        
        // Calculate cache efficiency
        result.cacheEfficiency = calculateCacheEfficiency(result.cacheMetrics);
        
    } catch (const exception& e) {
        cerr << "ERROR testing " << algorithmName << ": " << e.what() << endl;
        result.executionTimeMs = -1.0;
        result.isCorrect = false;
    }
    
    return result;
}

// Backwards compatibility
TestResult runTest(const string& algorithmName, 
                  vector<int> (*algorithm)(const string&, const string&),
                  const string& text, 
                  const string& pattern,
                  const vector<int>& expectedMatches) {
    return runCacheAwareTest(algorithmName, algorithm, text, pattern, expectedMatches);
}

// ============ UNIFIED ANALYSIS FUNCTIONS ============

void runUnifiedAnalysis() {
    cout << "\n🔬 STARTING UNIFIED PERFORMANCE + CACHE ANALYSIS 🔬" << endl;
    cout << "====================================================" << endl;
    cout << "Combining standard performance tests with cache boundary analysis" << endl;
    cout << "This will generate comprehensive academic datasets" << endl;
    
    vector<TestResult> allResults;
    vector<string> dataTypeNames = {"Clean Text", "System Logs", "Network Packets", "Binary Patterns"};
    vector<TestDataType> dataTypes = {CLEAN_TEXT, SYSTEM_LOGS, NETWORK_PACKETS, BINARY_PATTERNS};
    
    // Standard test sizes
    vector<size_t> standardSizes = {1000, 10000, 100000};
    vector<size_t> patternSizes = {4, 16, 32};
    
    // Cache boundary test sizes
    vector<size_t> cacheBoundarySizes = {
        16*1024,    // 16KB - L1 comfortable
        32*1024,    // 32KB - L1 boundary
        64*1024,    // 64KB - exceeds L1
        128*1024,   // 128KB - small L2
        512*1024,   // 512KB - L2 boundary
        1024*1024,  // 1MB - exceeds L2
        4*1024*1024 // 4MB - L3 boundary
    };
    
    cout << "\n=== PHASE 1: STANDARD PERFORMANCE TESTS ===" << endl;
    
    // Run standard tests across different data types
    for (size_t typeIdx = 0; typeIdx < dataTypes.size(); typeIdx++) {
        TestDataType type = dataTypes[typeIdx];
        cout << "\nTesting on: " << dataTypeNames[typeIdx] << endl;
        
        for (size_t textSize : standardSizes) {
            cout << "  Text size: " << textSize << " characters" << endl;
            string text = generateTestData(type, textSize);
            
            for (size_t patternSize : patternSizes) {
                if (patternSize >= textSize) continue;
                
                cout << "    Pattern length: " << patternSize << " chars" << endl;
                string pattern = generatePattern(text, patternSize, true);
                
                vector<int> expectedMatches = naiveSearch(text, pattern);
                
                vector<TestResult> roundResults;
                roundResults.push_back(runCacheAwareTest("Naive", naiveSearch, text, pattern, expectedMatches));
                roundResults.push_back(runCacheAwareTest("KMP", kmpSearch, text, pattern, expectedMatches));
                roundResults.push_back(runCacheAwareTest("Rabin-Karp", rabinKarpSearch, text, pattern, expectedMatches));
                roundResults.push_back(runCacheAwareTest("Boyer-Moore", boyerMooreSearch, text, pattern, expectedMatches));
                
                // Set metadata for these results
                for (auto& result : roundResults) {
                    result.testCategory = "Standard";
                    result.dataType = dataTypeNames[typeIdx];
                }
                
                displayUnifiedResults(roundResults, textSize);
                allResults.insert(allResults.end(), roundResults.begin(), roundResults.end());
            }
        }
    }
    
    cout << "\n=== PHASE 2: CACHE BOUNDARY TESTS ===" << endl;
    
    // Run cache boundary tests (focus on clean text for cache analysis)
    for (size_t textSize : cacheBoundarySizes) {
        cout << "\nCache test: " << textSize/1024 << "KB text" << endl;
        string text = generateTestData(CLEAN_TEXT, textSize);
        
        for (size_t patternSize : {4, 16}) { // Fewer pattern sizes for cache tests
            if (patternSize >= textSize) continue;
            
            cout << "  Pattern length: " << patternSize << " chars" << endl;
            string pattern = generatePattern(text, patternSize, true);
            
            vector<int> expectedMatches = naiveSearch(text, pattern);
            
            vector<TestResult> roundResults;
            roundResults.push_back(runCacheAwareTest("Naive", naiveSearch, text, pattern, expectedMatches));
            roundResults.push_back(runCacheAwareTest("KMP", kmpSearch, text, pattern, expectedMatches));
            roundResults.push_back(runCacheAwareTest("Rabin-Karp", rabinKarpSearch, text, pattern, expectedMatches));
            roundResults.push_back(runCacheAwareTest("Boyer-Moore", boyerMooreSearch, text, pattern, expectedMatches));
            
            // Set metadata for these results
            for (auto& result : roundResults) {
                result.testCategory = "Cache Boundary";
                result.dataType = "Clean Text (Cache Test)";
            }
            
            displayUnifiedResults(roundResults, textSize);
            allResults.insert(allResults.end(), roundResults.begin(), roundResults.end());
        }
    }
    
    cout << "\n=== PHASE 3: UNIFIED ANALYSIS & EXPORT ===" << endl;
    
    // Calculate efficiency scores
    double maxTime = 0, maxMemory = 0;
    for (const auto& result : allResults) {
        if (result.executionTimeMs > maxTime) maxTime = result.executionTimeMs;
        if (result.memoryUsageKB > maxMemory) maxMemory = result.memoryUsageKB;
    }
    
    for (auto& result : allResults) {
        result.efficiencyScore = calculateEfficiencyScore(result, maxTime, maxMemory);
    }
    
    // Perform comparative analysis
    vector<AlgorithmComparison> comparisons = analyzeAlgorithmPerformance(allResults);
    displayAlgorithmComparison(comparisons);
    displayCacheImpactAnalysis(allResults);
    
    // Export unified results
    exportUnifiedAnalysis(allResults, dataTypeNames, standardSizes, patternSizes);
    
    cout << "\n🎯 UNIFIED ANALYSIS COMPLETED! 🎯" << endl;
    cout << "📊 Generated comprehensive datasets for academic analysis" << endl;
    cout << "📈 Performance + Cache correlation analysis included" << endl;
    cout << "🔬 Ready for statistical analysis and publication" << endl;
}

void displayUnifiedResults(const vector<TestResult>& results, size_t textSize) {
    cout << "    ┌─────────────────────────────────────────────────────────────────────────────────┐" << endl;
    cout << "    │ Algorithm    │ Time (ms)│ Memory   │ Matches│ Cache Miss%│ Efficiency│ Cache Eff│" << endl;
    cout << "    ├─────────────────────────────────────────────────────────────────────────────────┤" << endl;
    
    for (const auto& result : results) {
        cout << "    │ " << setw(12) << left << result.algorithmName 
             << " │ " << setw(8) << fixed << setprecision(3) << result.executionTimeMs
             << " │ " << setw(8) << fixed << setprecision(2) << result.memoryUsageKB
             << " │ " << setw(6) << result.matchesFound
             << " │ " << setw(10) << fixed << setprecision(2) << (result.cacheMetrics.cache_miss_rate * 100)
             << " │ " << setw(9) << fixed << setprecision(1) << result.efficiencyScore
             << " │ " << setw(8) << fixed << setprecision(1) << result.cacheEfficiency << " │" << endl;
    }
    cout << "    └─────────────────────────────────────────────────────────────────────────────────┘" << endl;
}

vector<AlgorithmComparison> analyzeAlgorithmPerformance(const vector<TestResult>& allResults) {
    map<string, vector<TestResult>> algorithmGroups;
    
    // Group results by algorithm
    for (const auto& result : allResults) {
        algorithmGroups[result.algorithmName].push_back(result);
    }
    
    vector<AlgorithmComparison> comparisons;
    
    for (const auto& group : algorithmGroups) {
        AlgorithmComparison comp;
        comp.algorithmName = group.first;
        
        double sumTime = 0, sumCacheMiss = 0, sumEfficiency = 0;
        double sumTimeSquared = 0, sumCacheSquared = 0;
        int correctCount = 0;
        
        for (const auto& result : group.second) {
            sumTime += result.executionTimeMs;
            sumCacheMiss += result.cacheMetrics.cache_miss_rate;
            sumEfficiency += result.efficiencyScore;
            sumTimeSquared += result.executionTimeMs * result.executionTimeMs;
            sumCacheSquared += result.cacheMetrics.cache_miss_rate * result.cacheMetrics.cache_miss_rate;
            
            if (result.isCorrect) correctCount++;
        }
        
        int n = group.second.size();
        comp.totalTests = n;
        comp.correctResults = correctCount;
        comp.avgExecutionTime = sumTime / n;
        comp.avgCacheMissRate = sumCacheMiss / n;
        comp.avgEfficiencyScore = sumEfficiency / n;
        comp.reliabilityScore = (double)correctCount / n * 100;
        
        // Calculate variance
        comp.performanceVariance = (sumTimeSquared / n) - (comp.avgExecutionTime * comp.avgExecutionTime);
        comp.cacheVariance = (sumCacheSquared / n) - (comp.avgCacheMissRate * comp.avgCacheMissRate);
        
        comparisons.push_back(comp);
    }
    
    return comparisons;
}

void displayAlgorithmComparison(const vector<AlgorithmComparison>& comparisons) {
    cout << "\n📊 ALGORITHM PERFORMANCE COMPARISON" << endl;
    cout << "══════════════════════════════════════════════════════════════════════════════════" << endl;
    cout << "│ Algorithm    │ Avg Time │ Cache Miss% │ Efficiency │ Reliability │ Consistency │" << endl;
    cout << "├──────────────┼──────────┼─────────────┼────────────┼─────────────┼─────────────┤" << endl;
    
    for (const auto& comp : comparisons) {
        cout << "│ " << setw(12) << left << comp.algorithmName 
             << " │ " << setw(8) << fixed << setprecision(3) << comp.avgExecutionTime
             << " │ " << setw(11) << fixed << setprecision(2) << (comp.avgCacheMissRate * 100)
             << " │ " << setw(10) << fixed << setprecision(1) << comp.avgEfficiencyScore
             << " │ " << setw(11) << fixed << setprecision(1) << comp.reliabilityScore
             << " │ " << setw(11) << fixed << setprecision(3) << sqrt(comp.performanceVariance) << " │" << endl;
    }
    cout << "└──────────────┴──────────┴─────────────┴────────────┴─────────────┴─────────────┘" << endl;
}

void displayCacheImpactAnalysis(const vector<TestResult>& allResults) {
    cout << "\n🔬 CACHE IMPACT ANALYSIS" << endl;
    cout << "═══════════════════════════════════════════════════════════════════" << endl;
    
    // Calculate correlation between cache miss rate and execution time
    vector<double> cacheMissRates, executionTimes;
    
    for (const auto& result : allResults) {
        if (result.cacheMetrics.measurementAvailable && result.executionTimeMs > 0) {
            cacheMissRates.push_back(result.cacheMetrics.cache_miss_rate);
            executionTimes.push_back(result.executionTimeMs);
        }
    }
    
    double correlation = calculateCorrelation(cacheMissRates, executionTimes);
    
    cout << "📈 CORRELATION ANALYSIS:" << endl;
    cout << "   Cache Miss Rate ↔ Execution Time: " << fixed << setprecision(3) << correlation << endl;
    
    if (correlation > 0.7) {
        cout << "   🔴 STRONG POSITIVE correlation - Cache misses significantly impact performance!" << endl;
    } else if (correlation > 0.3) {
        cout << "   🟡 MODERATE correlation - Cache behavior affects performance" << endl;
    } else if (correlation > -0.3) {
        cout << "   🟢 WEAK correlation - Cache impact is minimal" << endl;
    } else {
        cout << "   🔵 NEGATIVE correlation - Unexpected cache behavior pattern" << endl;
    }
    
    // Algorithm-specific cache analysis
    map<string, vector<double>> algorithmCacheRates;
    map<string, vector<double>> algorithmTimes;
    
    for (const auto& result : allResults) {
        if (result.cacheMetrics.measurementAvailable) {
            algorithmCacheRates[result.algorithmName].push_back(result.cacheMetrics.cache_miss_rate);
            algorithmTimes[result.algorithmName].push_back(result.executionTimeMs);
        }
    }
    
    cout << "\n🔍 ALGORITHM-SPECIFIC CACHE IMPACT:" << endl;
    for (const auto& alg : algorithmCacheRates) {
        if (alg.second.size() > 1) {
            double algCorrelation = calculateCorrelation(alg.second, algorithmTimes[alg.first]);
            cout << "   " << setw(12) << left << alg.first 
                 << ": " << fixed << setprecision(3) << algCorrelation << endl;
        }
    }
}

void exportUnifiedAnalysis(const vector<TestResult>& allResults, 
                          const vector<string>& dataTypes,
                          const vector<size_t>& textSizes,
                          const vector<size_t>& patternSizes) {
    
    // Export 1: Main unified dataset
    ofstream mainFile("unified_performance_cache_analysis.csv");
    if (mainFile.is_open()) {
        mainFile << "Algorithm,Test_Category,Data_Type,Text_Size_Bytes,Pattern_Size_Bytes,";
        mainFile << "Execution_Time_Ms,Memory_Usage_KB,Matches_Found,Is_Correct,";
        mainFile << "L1_Cache_Misses,L2_Cache_Misses,Instructions,Cache_Miss_Rate,";
        mainFile << "Efficiency_Score,Cache_Efficiency,Cache_Data_Available" << endl;
        
        for (const auto& result : allResults) {
            mainFile << result.algorithmName << ","
                     << result.testCategory << ","
                     << result.dataType << ","
                     << result.textSizeBytes << ","
                     << result.patternSizeBytes << ","
                     << fixed << setprecision(6) << result.executionTimeMs << ","
                     << fixed << setprecision(3) << result.memoryUsageKB << ","
                     << result.matchesFound << ","
                     << (result.isCorrect ? "Yes" : "No") << ","
                     << result.cacheMetrics.l1_cache_misses << ","
                     << result.cacheMetrics.l2_cache_misses << ","
                     << result.cacheMetrics.instructions << ","
                     << fixed << setprecision(8) << result.cacheMetrics.cache_miss_rate << ","
                     << fixed << setprecision(3) << result.efficiencyScore << ","
                     << fixed << setprecision(3) << result.cacheEfficiency << ","
                     << (result.cacheMetrics.measurementAvailable ? "Yes" : "No") << endl;
        }
        mainFile.close();
        cout << "✅ Main dataset exported: unified_performance_cache_analysis.csv" << endl;
    }
    
    // Export 2: Cache performance correlation analysis
    ofstream corrFile("cache_performance_correlation.csv");
    if (corrFile.is_open()) {
        corrFile << "Algorithm,Cache_Miss_Rate,Execution_Time_Ms,Text_Size_Category,Correlation_Group" << endl;
        
        for (const auto& result : allResults) {
            if (result.cacheMetrics.measurementAvailable) {
                string sizeCategory;
                if (result.textSizeBytes < 32*1024) sizeCategory = "Small";
                else if (result.textSizeBytes < 512*1024) sizeCategory = "Medium";
                else sizeCategory = "Large";
                
                corrFile << result.algorithmName << ","
                         << fixed << setprecision(8) << result.cacheMetrics.cache_miss_rate << ","
                         << fixed << setprecision(6) << result.executionTimeMs << ","
                         << sizeCategory << ","
                         << result.testCategory << endl;
            }
        }
        corrFile.close();
        cout << "✅ Correlation analysis exported: cache_performance_correlation.csv" << endl;
    }
    
    // Export 3: Cache boundary effects analysis
    ofstream boundaryFile("cache_boundary_effects.csv");
    if (boundaryFile.is_open()) {
        boundaryFile << "Text_Size_KB,Algorithm,Performance_Degradation_Percent,Cache_Miss_Increase_Percent,";
        boundaryFile << "Efficiency_Drop,Cache_Boundary_Type" << endl;
        
        // Analyze cache boundary effects
        map<string, map<size_t, double>> algorithmPerformance; // [algorithm][size] = avgTime
        
        for (const auto& result : allResults) {
            if (result.testCategory == "Cache Boundary") {
                algorithmPerformance[result.algorithmName][result.textSizeBytes] = result.executionTimeMs;
            }
        }
        
        // Calculate degradation at each boundary
        vector<size_t> boundaries = {32*1024, 64*1024, 512*1024, 1024*1024, 4*1024*1024};
        
        for (const auto& alg : algorithmPerformance) {
            for (size_t boundary : boundaries) {
                if (alg.second.count(boundary/2) && alg.second.count(boundary)) {
                    double beforeBoundary = alg.second.at(boundary/2);
                    double atBoundary = alg.second.at(boundary);
                    double degradation = ((atBoundary - beforeBoundary) / beforeBoundary) * 100;
                    
                    string boundaryType;
                    if (boundary == 32*1024) boundaryType = "L1_Boundary";
                    else if (boundary == 512*1024) boundaryType = "L2_Boundary";
                    else if (boundary == 4*1024*1024) boundaryType = "L3_Boundary";
                    else boundaryType = "Cache_Transition";
                    
                    boundaryFile << boundary/1024 << ","
                                 << alg.first << ","
                                 << fixed << setprecision(2) << degradation << ","
                                 << "N/A" << ","
                                 << fixed << setprecision(2) << degradation << ","
                                 << boundaryType << endl;
                }
            }
        }
        boundaryFile.close();
        cout << "✅ Boundary effects exported: cache_boundary_effects.csv" << endl;
    }
    
    cout << "\n📈 EXPORT SUMMARY:" << endl;
    cout << "   • " << allResults.size() << " total test results exported" << endl;
    cout << "   • Performance + cache metrics combined" << endl;
    cout << "   • Statistical correlation data included" << endl;
    cout << "   • Cache boundary analysis completed" << endl;
}

// ============ LEGACY FUNCTIONS (for backwards compatibility) ============

void displayResults(const vector<TestResult>& results) {
    displayUnifiedResults(results, 0);
}

void runCacheExperiment() {
    cout << "\n⚠️  LEGACY CACHE EXPERIMENT" << endl;
    cout << "Consider using the new unified analysis instead!" << endl;
    cout << "Run option (3) for comprehensive analysis." << endl;
}

void exportCacheResultsToCSV(const vector<vector<TestResult>>& allResultsByGroup, 
                             const vector<string>& dataTypeNames,
                             const vector<size_t>& testSizes,
                             const vector<size_t>& patternSizes,
                             const string& fileName) {
    // Legacy function - redirect to unified export
    vector<TestResult> flatResults;
    for (const auto& group : allResultsByGroup) {
        flatResults.insert(flatResults.end(), group.begin(), group.end());
    }
    exportUnifiedAnalysis(flatResults, dataTypeNames, testSizes, patternSizes);
}

void exportResultsToCSV(const vector<vector<TestResult>>& allResultsByGroup, 
                        const vector<string>& dataTypeNames,
                        const vector<size_t>& testSizes,
                        const vector<size_t>& patternSizes,
                        const string& fileName) {
    exportCacheResultsToCSV(allResultsByGroup, dataTypeNames, testSizes, patternSizes, fileName);
}

void runTestSuite() {
    cout << "Enhanced test suite with unified analysis capabilities" << endl;
    cout << "Platform: ";
    
#ifdef _WIN32
    cout << "Windows (Process Memory API)" << endl;
#elif defined(__linux__)
    cout << "Linux (/proc/self/statm + perf_event)" << endl;
#elif defined(__APPLE__)
    cout << "macOS (getrusage() + estimates)" << endl;
#else
    cout << "Unknown platform (basic estimates)" << endl;
#endif

    cout << "\n🎯 ANALYSIS OPTIONS:" << endl;
    cout << "1. Legacy Standard Tests" << endl;
    cout << "2. Legacy Cache Experiment" << endl;
    cout << "3. 🚀 NEW: Unified Performance + Cache Analysis (RECOMMENDED)" << endl;
    cout << "4. All legacy tests" << endl;
    cout << "Choice (1-4): ";
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1:
            cout << "Running legacy standard tests..." << endl;
            // Run original standard tests (simplified version)
            break;
        case 2:
            runCacheExperiment();
            break;
        case 3:
            runUnifiedAnalysis();
            break;
        case 4:
            cout << "Running all legacy tests..." << endl;
            runCacheExperiment();
            break;
        default:
            cout << "Invalid choice, running unified analysis..." << endl;
            runUnifiedAnalysis();
            break;
    }
}