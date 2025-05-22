#include "TestFramework.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>

using namespace std;
using namespace std::chrono;

namespace TestUtils {

    // Provjera konzistentnosti rezultata između algoritama
    bool verifyConsistency(const vector<TestResult>& results) {
        if (results.size() < 2) return true;
        
        int expectedMatches = results[0].matchesFound;
        for (const auto& result : results) {
            if (result.matchesFound != expectedMatches) {
                cout << "GREŠKA: Nekonzistentni rezultati! " 
                     << result.algorithmName << " pronašao " 
                     << result.matchesFound << " podudaranja, očekivano " 
                     << expectedMatches << endl;
                return false;
            }
        }
        return true;
    }
    
    // Generisanje ponavljajućeg pattern-a
    string generateRepeatingPattern(size_t length, const string& basePattern) {
        string result;
        result.reserve(length);
        
        while (result.length() < length) {
            result += basePattern;
        }
        
        if (result.length() > length) {
            result.resize(length);
        }
        
        return result;
    }
    
    // Generisanje random pattern-a
    string generateRandomPattern(size_t length, char minChar, char maxChar) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(minChar, maxChar);
        
        string pattern;
        pattern.reserve(length);
        
        for (size_t i = 0; i < length; i++) {
            pattern += static_cast<char>(dist(gen));
        }
        
        return pattern;
    }
    
    // Računanje statistika performansi
    PerformanceStats calculateStats(const vector<double>& times) {
        PerformanceStats stats;
        
        if (times.empty()) return stats;
        
        stats.minTime = *min_element(times.begin(), times.end());
        stats.maxTime = *max_element(times.begin(), times.end());
        stats.avgTime = accumulate(times.begin(), times.end(), 0.0) / times.size();
        
        // Standardna devijacija
        double variance = 0.0;
        for (double time : times) {
            variance += pow(time - stats.avgTime, 2);
        }
        variance /= times.size();
        stats.stdDev = sqrt(variance);
        
        return stats;
    }
    
    // Detaljni benchmark
    void runBenchmark(const string& algorithmName,
                     vector<int> (*algorithm)(const string&, const string&),
                     const string& text,
                     const string& pattern,
                     int iterations) {
        
        cout << "\n=== DETALJNI BENCHMARK: " << algorithmName << " ===" << endl;
        cout << "Tekst: " << text.length() << " karaktera" << endl;
        cout << "Pattern: " << pattern.length() << " karaktera" << endl;
        cout << "Broj iteracija: " << iterations << endl;
        cout << "----------------------------------------" << endl;
        
        vector<double> times;
        vector<int> matchCounts;
        
        // Zagrijavanje (warm-up)
        for (int i = 0; i < 3; i++) {
            algorithm(text, pattern);
        }
        
        // Stvarni benchmark
        for (int i = 0; i < iterations; i++) {
            auto start = high_resolution_clock::now();
            vector<int> matches = algorithm(text, pattern);
            auto stop = high_resolution_clock::now();
            
            auto duration = duration_cast<nanoseconds>(stop - start);
            times.push_back(duration.count() / 1000000.0); // u milisekundama
            matchCounts.push_back(matches.size());
            
            // Progress indicator
            if ((i + 1) % (iterations / 10) == 0) {
                cout << "Progres: " << ((i + 1) * 100 / iterations) << "%" << endl;
            }
        }
        
        // Analiza rezultata
        PerformanceStats stats = calculateStats(times);
        
        cout << "\n=== REZULTATI BENCHMARKA ===" << endl;
        cout << fixed << setprecision(3);
        cout << "Min vrijeme:  " << stats.minTime << " ms" << endl;
        cout << "Max vrijeme:  " << stats.maxTime << " ms" << endl;
        cout << "Avg vrijeme:  " << stats.avgTime << " ms" << endl;
        cout << "Std. dev:     " << stats.stdDev << " ms" << endl;
        cout << "Pronađeno:    " << matchCounts[0] << " podudaranja" << endl;
        
        // Provjera konzistentnosti
        bool consistent = all_of(matchCounts.begin(), matchCounts.end(),
                                [&](int count) { return count == matchCounts[0]; });
        cout << "Konzistentnost: " << (consistent ? "DA" : "NE") << endl;
        
        // Percentili
        vector<double> sortedTimes = times;
        sort(sortedTimes.begin(), sortedTimes.end());
        
        cout << "\n=== PERCENTILI ===" << endl;
        cout << "P50 (medijan): " << sortedTimes[sortedTimes.size() / 2] << " ms" << endl;
        cout << "P90:           " << sortedTimes[(sortedTimes.size() * 90) / 100] << " ms" << endl;
        cout << "P95:           " << sortedTimes[(sortedTimes.size() * 95) / 100] << " ms" << endl;
        cout << "P99:           " << sortedTimes[(sortedTimes.size() * 99) / 100] << " ms" << endl;
        
        cout << "========================================" << endl;
    }
    
    // Specifični testovi pattern-a
    vector<TestResult> runPatternSpecificTests() {
        vector<TestResult> results;
        
        // Test 1: Vrlo kratak pattern
        cout << "Pattern Test 1: Vrlo kratak pattern (1 karakter)" << endl;
        string text1 = generateRepeatingPattern(10000, "abcdefghij");
        string pattern1 = "e";
        
        vector<int> expected1 = naiveSearch(text1, pattern1);
        results.push_back(runTest("KMP (kratak)", kmpSearch, text1, pattern1, expected1));
        results.push_back(runTest("Boyer-Moore (kratak)", boyerMooreSearch, text1, pattern1, expected1));
        
        // Test 2: Ponavljajući pattern
        cout << "Pattern Test 2: Ponavljajući pattern" << endl;
        string text2 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        string pattern2 = "aaa";
        
        vector<int> expected2 = naiveSearch(text2, pattern2);
        results.push_back(runTest("KMP (ponavljanje)", kmpSearch, text2, pattern2, expected2));
        results.push_back(runTest("Rabin-Karp (ponavljanje)", rabinKarpSearch, text2, pattern2, expected2));
        
        // Test 3: Pattern na početku teksta
        cout << "Pattern Test 3: Pattern na početku" << endl;
        string text3 = "abcdefghijklmnopqrstuvwxyz";
        string pattern3 = "abc";
        
        vector<int> expected3 = naiveSearch(text3, pattern3);
        results.push_back(runTest("Svi algoritmi (početak)", naiveSearch, text3, pattern3, expected3));
        
        // Test 4: Pattern na kraju teksta
        cout << "Pattern Test 4: Pattern na kraju" << endl;
        string pattern4 = "xyz";
        
        vector<int> expected4 = naiveSearch(text3, pattern4);
        results.push_back(runTest("Svi algoritmi (kraj)", naiveSearch, text3, pattern4, expected4));
        
        return results;
    }
}

// Implementacija custom test suite-a
void runCustomTestSuite(const TestConfiguration& config) {
    cout << "Pokretanje custom test suite-a..." << endl;
    cout << "Konfiguracija:" << endl;
    cout << "- Tipovi podataka: " << config.dataTypes.size() << endl;
    cout << "- Veličine teksta: ";
    for (size_t size : config.textSizes) cout << size << " ";
    cout << endl;
    cout << "- Veličine pattern-a: ";
    for (size_t size : config.patternSizes) cout << size << " ";
    cout << endl;
    cout << "- Broj iteracija: " << config.numIterations << endl;
    cout << "- Memory profiling: " << (config.enableMemoryProfiling ? "DA" : "NE") << endl;
    cout << "- Stress testovi: " << (config.enableStressTests ? "DA" : "NE") << endl;
    cout << "========================================" << endl;
    
    vector<vector<TestResult>> allResults;
    vector<string> dataTypeNames = {"Čisti tekst", "Sistemski logovi", "Mrežni paketi", "Binarni uzorci"};
    
    // Osnovni testovi
    if (find(config.categories.begin(), config.categories.end(), BASIC_TESTS) != config.categories.end()) {
        cout << "\n=== OSNOVNI TESTOVI ===" << endl;
        
        for (TestDataType type : config.dataTypes) {
            for (size_t textSize : config.textSizes) {
                for (size_t patternSize : config.patternSizes) {
                    if (patternSize >= textSize) continue;
                    
                    string text = generateTestData(type, textSize);
                    string pattern = generatePattern(text, patternSize, true);
                    
                    vector<int> expected = naiveSearch(text, pattern);
                    vector<TestResult> results;
                    
                    results.push_back(runTest("Naivni", naiveSearch, text, pattern, expected));
                    results.push_back(runTest("KMP", kmpSearch, text, pattern, expected));
                    results.push_back(runTest("Rabin-Karp", rabinKarpSearch, text, pattern, expected));
                    results.push_back(runTest("Boyer-Moore", boyerMooreSearch, text, pattern, expected));
                    
                    if (!TestUtils::verifyConsistency(results)) {
                        cout << "UPOZORENJE: Pronađene nekonzistentnosti!" << endl;
                    }
                    
                    displayResults(results);
                    allResults.push_back(results);
                }
            }
        }
    }
    
    // Edge case testovi
    if (find(config.categories.begin(), config.categories.end(), EDGE_CASE_TESTS) != config.categories.end()) {
        cout << "\n=== EDGE CASE TESTOVI ===" << endl;
        vector<TestResult> edgeResults = runEdgeCaseTests();
        displayResults(edgeResults);
        allResults.push_back(edgeResults);
    }
    
    // Pattern-specifični testovi
    if (find(config.categories.begin(), config.categories.end(), PATTERN_TESTS) != config.categories.end()) {
        cout << "\n=== PATTERN-SPECIFIČNI TESTOVI ===" << endl;
        vector<TestResult> patternResults = TestUtils::runPatternSpecificTests();
        displayResults(patternResults);
        allResults.push_back(patternResults);
    }
    
    // Stress testovi
    if (config.enableStressTests && 
        find(config.categories.begin(), config.categories.end(), STRESS_TESTS) != config.categories.end()) {
        cout << "\n=== STRESS TESTOVI ===" << endl;
        
        vector<size_t> stressSizes = {500000, 1000000}; // 500K i 1M karaktera
        for (size_t stressSize : stressSizes) {
            cout << "Stress test sa " << stressSize << " karaktera..." << endl;
            
            vector<TestResult> stressResults;
            stressResults.push_back(runStressTest("Naivni", naiveSearch, stressSize));
            stressResults.push_back(runStressTest("KMP", kmpSearch, stressSize));
            stressResults.push_back(runStressTest("Rabin-Karp", rabinKarpSearch, stressSize));
            stressResults.push_back(runStressTest("Boyer-Moore", boyerMooreSearch, stressSize));
            
            displayResults(stressResults);
            allResults.push_back(stressResults);
        }
    }
    
    // Izvoz rezultata
    exportResultsToCSV(allResults, dataTypeNames, config.textSizes, config.patternSizes, config.outputFileName);
    
    cout << "\n=== CUSTOM TEST SUITE ZAVRŠEN ===" << endl;
    cout << "Ukupno grupa testova: " << allResults.size() << endl;
    cout << "Rezultati izvezeni u: " << config.outputFileName << endl;
}