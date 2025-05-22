#ifndef TESTFRAMEWORK_H
#define TESTFRAMEWORK_H

#include <string>
#include <vector>

// Tipovi testnih podataka
enum TestDataType {
    CLEAN_TEXT,      // Čist alfanumerički tekst
    SYSTEM_LOGS,     // Formatirani sistemski logovi
    NETWORK_PACKETS, // Simulirani mrežni paketi (binarni)
    BINARY_PATTERNS  // Maliciozni binarni uzorci
};

// Struktura za čuvanje rezultata testa
struct TestResult {
    std::string algorithmName;  // Naziv algoritma
    double executionTimeMs;     // Vrijeme izvršavanja u milisekundama
    int matchesFound;           // Broj pronađenih podudaranja
    double memoryUsageKB;       // Potrošnja memorije u KB
    bool isCorrect;             // Da li je rezultat tačan
    
    // Konstruktor
    TestResult() : algorithmName(""), executionTimeMs(0.0), 
                   matchesFound(0), memoryUsageKB(0.0), isCorrect(false) {}
};

// Struktura za mjerenje memorije
struct MemorySnapshot {
    size_t virtualMemoryKB;
    size_t physicalMemoryKB;
    size_t peakMemoryKB;
    
    MemorySnapshot() : virtualMemoryKB(0), physicalMemoryKB(0), peakMemoryKB(0) {}
};

// Forward deklaracije funkcija iz algorithms
std::vector<int> naiveSearch(const std::string& text, const std::string& pattern);
std::vector<int> kmpSearch(const std::string& text, const std::string& pattern);
std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern);
std::vector<int> boyerMooreSearch(const std::string& text, const std::string& pattern);

// Funkcije za mjerenje memorije
MemorySnapshot getCurrentMemoryUsage();

// Funkcije za generisanje testnih podataka
std::string generateTestData(TestDataType type, size_t length);
std::string generatePattern(const std::string& text, size_t patternLength, bool ensureMatch = true);

// Funkcije za testiranje
TestResult runTest(const std::string& algorithmName, 
                  std::vector<int> (*algorithm)(const std::string&, const std::string&),
                  const std::string& text, 
                  const std::string& pattern,
                  const std::vector<int>& expectedMatches = {});

TestResult runStressTest(const std::string& algorithmName,
                        std::vector<int> (*algorithm)(const std::string&, const std::string&),
                        size_t textSize);

std::vector<TestResult> runEdgeCaseTests();

// Funkcije za prikaz i izvoz rezultata
void displayResults(const std::vector<TestResult>& results);
void exportResultsToCSV(const std::vector<std::vector<TestResult>>& allResultsByGroup, 
                        const std::vector<std::string>& dataTypeNames,
                        const std::vector<size_t>& testSizes,
                        const std::vector<size_t>& patternSizes,
                        const std::string& fileName);

// Glavna funkcija test suite-a
void runTestSuite();

// Pomoćne funkcije za napredne testove
namespace TestUtils {
    // Provjera konzistentnosti rezultata između algoritama
    bool verifyConsistency(const std::vector<TestResult>& results);
    
    // Generisanje različitih tipova pattern-a
    std::string generateRepeatingPattern(size_t length, const std::string& basePattern);
    std::string generateRandomPattern(size_t length, char minChar = 'a', char maxChar = 'z');
    
    // Statistička analiza performansi
    struct PerformanceStats {
        double minTime;
        double maxTime;
        double avgTime;
        double stdDev;
        
        PerformanceStats() : minTime(0), maxTime(0), avgTime(0), stdDev(0) {}
    };
    
    PerformanceStats calculateStats(const std::vector<double>& times);
    
    // Benchmark funkcija za detaljnu analizu
    void runBenchmark(const std::string& algorithmName,
                     std::vector<int> (*algorithm)(const std::string&, const std::string&),
                     const std::string& text,
                     const std::string& pattern,
                     int iterations = 100);
}

// Dodatne testne kategorije
enum TestCategory {
    BASIC_TESTS,        // Osnovni testovi
    EDGE_CASE_TESTS,    // Granični slučajevi
    STRESS_TESTS,       // Testovi pod opterećenjem
    ACCURACY_TESTS,     // Testovi tačnosti
    MEMORY_TESTS,       // Testovi memorije
    PATTERN_TESTS       // Specifični testovi pattern-a
};

// Konfiguracija testova
struct TestConfiguration {
    std::vector<TestDataType> dataTypes;
    std::vector<size_t> textSizes;
    std::vector<size_t> patternSizes;
    std::vector<TestCategory> categories;
    bool enableStressTests;
    bool enableMemoryProfiling;
    int numIterations;
    std::string outputFileName;
    
    // Konstruktor sa default vrijednostima
    TestConfiguration() : 
        dataTypes({CLEAN_TEXT, SYSTEM_LOGS, NETWORK_PACKETS, BINARY_PATTERNS}),
        textSizes({1000, 10000, 100000}),
        patternSizes({3, 10, 20}),
        categories({BASIC_TESTS, EDGE_CASE_TESTS, STRESS_TESTS}),
        enableStressTests(true),
        enableMemoryProfiling(true),
        numIterations(5),
        outputFileName("rezultati_testiranja_poboljsani.csv") {}
};

// Funkcija za pokretanje testova sa custom konfiguracijón
void runCustomTestSuite(const TestConfiguration& config);

#endif // TESTFRAMEWORK_H