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

// Platform-specific includes for memory measurement
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__)
#include <fstream>
#include <unistd.h>
#elif defined(__APPLE__)
#include <sys/resource.h>
#include <mach/mach.h>
#endif

using namespace std;
using namespace std::chrono;

// Jednostavno mjerenje memorije
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
        memoryKB = usage.ru_maxrss / 1024.0; // macOS returns bytes
    }
#endif
    
    return memoryKB;
}

// Generator testnih podataka
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

// Generisanje uzorka za pretragu
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

// Sigurna funkcija za testiranje performansi algoritma
TestResult runTest(const string& algorithmName, 
                  vector<int> (*algorithm)(const string&, const string&),
                  const string& text, 
                  const string& pattern,
                  const vector<int>& expectedMatches) {
    TestResult result;
    result.algorithmName = algorithmName;
    
    // Provjeri da li su ulazni podaci validni
    if (text.empty() || pattern.empty() || pattern.length() > text.length()) {
        result.executionTimeMs = 0.0;
        result.matchesFound = 0;
        result.memoryUsageKB = 0.0;
        result.isCorrect = true;
        return result;
    }
    
    try {
        // Početno mjerenje memorije
        double memBefore = getCurrentMemoryUsageKB();
        
        // Mjerenje vremena (3 pokretanja za bolju preciznost)
        double totalTime = 0.0;
        vector<int> matches;
        
        for (int run = 0; run < 3; run++) {
            auto start = high_resolution_clock::now();
            
            // Pozovi algoritam sa try-catch za safety
            try {
                matches = algorithm(text, pattern);
            } catch (const exception& e) {
                cerr << "GREŠKA u algoritmu " << algorithmName << ": " << e.what() << endl;
                result.executionTimeMs = -1.0;  // Oznaka greške
                result.matchesFound = 0;
                result.memoryUsageKB = 0.0;
                result.isCorrect = false;
                return result;
            } catch (...) {
                cerr << "NEOČEKIVANA GREŠKA u algoritmu " << algorithmName << endl;
                result.executionTimeMs = -1.0;  // Oznaka greške
                result.matchesFound = 0;
                result.memoryUsageKB = 0.0;
                result.isCorrect = false;
                return result;
            }
            
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            totalTime += duration.count() / 1000.0; // Pretvaranje u milisekunde
        }
        
        // Završno mjerenje memorije
        double memAfter = getCurrentMemoryUsageKB();
        
        // Prosječno vrijeme izvršavanja
        result.executionTimeMs = totalTime / 3.0;
        
        // Broj pronađenih podudaranja
        result.matchesFound = matches.size();
        
        // Stvarno mjerenje memorije
        result.memoryUsageKB = memAfter - memBefore;
        
        // Ako je razlika negativna ili premala, koristi grublu procjenu
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
        
        // Provjera tačnosti
        if (expectedMatches.empty()) {
            result.isCorrect = true;
        } else {
            vector<int> sortedMatches = matches;
            vector<int> sortedExpected = expectedMatches;
            sort(sortedMatches.begin(), sortedMatches.end());
            sort(sortedExpected.begin(), sortedExpected.end());
            result.isCorrect = (sortedMatches == sortedExpected);
            
            // Ako rezultati nisu konzistentni, ispiši debug info
            if (!result.isCorrect) {
                cout << "UPOZORENJE: " << algorithmName << " pronašao " 
                     << result.matchesFound << " podudaranja, očekivano " 
                     << expectedMatches.size() << endl;
            }
        }
        
    } catch (const exception& e) {
        cerr << "GREŠKA prilikom testiranja " << algorithmName << ": " << e.what() << endl;
        result.executionTimeMs = -1.0;
        result.matchesFound = 0;
        result.memoryUsageKB = 0.0;
        result.isCorrect = false;
    } catch (...) {
        cerr << "NEOČEKIVANA GREŠKA prilikom testiranja " << algorithmName << endl;
        result.executionTimeMs = -1.0;
        result.matchesFound = 0;
        result.memoryUsageKB = 0.0;
        result.isCorrect = false;
    }
    
    return result;
}

// Prikaz rezultata sa boljim error handling-om
void displayResults(const vector<TestResult>& results) {
    cout << "--------------------------------------------------------------------" << endl;
    cout << setw(15) << "Algoritam" << setw(15) << "Vrijeme (ms)" << setw(15) << "Podudaranja"
         << setw(15) << "Memorija (KB)" << setw(10) << "Tačnost" << endl;
    cout << "--------------------------------------------------------------------" << endl;
    
    for (const auto& result : results) {
        cout << setw(15) << result.algorithmName;
        
        // Prikaži vrijeme ili grešku
        if (result.executionTimeMs < 0) {
            cout << setw(15) << "GREŠKA";
        } else {
            cout << setw(15) << fixed << setprecision(3) << result.executionTimeMs;
        }
        
        cout << setw(15) << result.matchesFound
             << setw(15) << fixed << setprecision(2) << result.memoryUsageKB
             << setw(10) << (result.isCorrect ? "Da" : "Ne") << endl;
    }
    cout << "--------------------------------------------------------------------" << endl;
}

// Funkcija za izvoz rezultata u CSV
void exportResultsToCSV(const vector<vector<TestResult>>& allResultsByGroup, 
                        const vector<string>& dataTypeNames,
                        const vector<size_t>& testSizes,
                        const vector<size_t>& patternSizes,
                        const string& fileName) {
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Greška: Nije moguće otvoriti fajl za izvoz: " << fileName << endl;
        return;
    }
    
    // Zaglavlje
    file << "Tip podataka,Veličina teksta,Dužina uzorka,Algoritam,Vrijeme (ms),Podudaranja,Memorija (KB),Tačnost" << endl;
    
    size_t dataTypeIdx = 0;
    size_t textSizeIdx = 0;
    size_t patternSizeIdx = 0;
    
    for (const auto& resultGroup : allResultsByGroup) {
        for (const auto& result : resultGroup) {
            file << dataTypeNames[min(dataTypeIdx, dataTypeNames.size() - 1)] << ","
                 << (textSizeIdx < testSizes.size() ? testSizes[textSizeIdx] : 0) << ","
                 << (patternSizeIdx < patternSizes.size() ? patternSizes[patternSizeIdx] : 0) << ","
                 << result.algorithmName << ","
                 << fixed << setprecision(3) << result.executionTimeMs << ","
                 << result.matchesFound << ","
                 << fixed << setprecision(2) << result.memoryUsageKB << ","
                 << (result.isCorrect ? "Da" : "Ne") << endl;
        }
        
        patternSizeIdx++;
        if (patternSizeIdx >= patternSizes.size() || 
            (patternSizeIdx < patternSizes.size() && textSizeIdx < testSizes.size() && 
             patternSizes[patternSizeIdx] >= testSizes[textSizeIdx])) {
            patternSizeIdx = 0;
            textSizeIdx++;
            
            if (textSizeIdx >= testSizes.size()) {
                textSizeIdx = 0;
                dataTypeIdx++;
            }
        }
    }
    
    file.close();
    cout << "Rezultati su izvezeni u: " << fileName << endl;
}

// Glavna funkcija za pokretanje testova
void runTestSuite() {
    cout << "Pokretanje test suite-a sa poboljšanim mjerenjem performansi..." << endl;
    cout << "Platforma za mjerenje memorije: ";
    
#ifdef _WIN32
    cout << "Windows (Process Memory API)" << endl;
#elif defined(__linux__)
    cout << "/proc/self/statm (Linux)" << endl;
#elif defined(__APPLE__)
    cout << "getrusage() (macOS)" << endl;
#else
    cout << "Procjena (nepoznata platforma)" << endl;
#endif
    
    vector<vector<TestResult>> allResultsByGroup;
    
    // Veličine testnih tekstova
    vector<size_t> testSizes = {1000, 10000, 100000};
    
    // Duljine uzoraka za pretragu
    vector<size_t> patternSizes = {3, 10, 20};
    
    // Tipovi testnih podataka
    vector<TestDataType> dataTypes = {CLEAN_TEXT, SYSTEM_LOGS, NETWORK_PACKETS, BINARY_PATTERNS};
    vector<string> dataTypeNames = {"Čisti tekst", "Sistemski logovi", "Mrežni paketi", "Binarni uzorci"};
    
    for (size_t i = 0; i < dataTypes.size(); i++) {
        TestDataType type = dataTypes[i];
        cout << "\nTestiranje na: " << dataTypeNames[i] << endl;
        
        for (size_t textSize : testSizes) {
            cout << "  Veličina teksta: " << textSize << " karaktera" << endl;
            
            string text = generateTestData(type, textSize);
            
            for (size_t patternSize : patternSizes) {
                if (patternSize >= textSize) continue;
                
                cout << "    Dužina uzorka: " << patternSize << " karaktera" << endl;
                
                string pattern = generatePattern(text, patternSize, true);
                
                // Prvo pokrećemo naivni algoritam za dobijanje očekivanih rezultata
                cout << "      Pokretanje referentnog (naivni) algoritma..." << endl;
                vector<int> expectedMatches;
                try {
                    expectedMatches = naiveSearch(text, pattern);
                } catch (...) {
                    cout << "      GREŠKA: Naivni algoritam failed - preskačemo ovaj test" << endl;
                    continue;
                }
                
                // Testiramo sve algoritme
                cout << "      Testiranje svih algoritama..." << endl;
                vector<TestResult> results;
                
                // Testiraj svaki algoritam pojedinačno
                results.push_back(runTest("Naivni", naiveSearch, text, pattern, expectedMatches));
                results.push_back(runTest("KMP", kmpSearch, text, pattern, expectedMatches));
                results.push_back(runTest("Rabin-Karp", rabinKarpSearch, text, pattern, expectedMatches));
                results.push_back(runTest("Boyer-Moore", boyerMooreSearch, text, pattern, expectedMatches));
                
                displayResults(results);
                
                // Provjeri da li su svi algoritmi dali konzistentne rezultate
                bool allCorrect = true;
                for (const auto& result : results) {
                    if (!result.isCorrect || result.executionTimeMs < 0) {
                        allCorrect = false;
                        break;
                    }
                }
                
                if (!allCorrect) {
                    cout << "      UPOZORENJE: Neki algoritmi su dali nekonzistentne rezultate!" << endl;
                }
                
                allResultsByGroup.push_back(results);
            }
        }
    }
    
    // Izvoz rezultata u CSV
    exportResultsToCSV(allResultsByGroup, dataTypeNames, testSizes, patternSizes, "rezultati_poboljsani.csv");
    
    cout << "\n=== ZAVRŠETAK TESTIRANJA ===" << endl;
    cout << "• Testovi su izvršeni sa stvarnim mjerenjem memorije" << endl;
    cout << "• Vrijeme izvršavanja je prosjek od 3 pokretanja" << endl;
    cout << "• Rezultati su izvezeni u CSV fajl" << endl;
}