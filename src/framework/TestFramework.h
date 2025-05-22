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

// Forward deklaracije funkcija iz algorithms
std::vector<int> naiveSearch(const std::string& text, const std::string& pattern);
std::vector<int> kmpSearch(const std::string& text, const std::string& pattern);
std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern);
std::vector<int> boyerMooreSearch(const std::string& text, const std::string& pattern);

// Funkcije za mjerenje memorije
double getCurrentMemoryUsageKB();

// Funkcije za generisanje testnih podataka
std::string generateTestData(TestDataType type, size_t length);
std::string generatePattern(const std::string& text, size_t patternLength, bool ensureMatch = true);

// Funkcije za testiranje
TestResult runTest(const std::string& algorithmName, 
                  std::vector<int> (*algorithm)(const std::string&, const std::string&),
                  const std::string& text, 
                  const std::string& pattern,
                  const std::vector<int>& expectedMatches = {});

// Funkcije za prikaz i izvoz rezultata
void displayResults(const std::vector<TestResult>& results);
void exportResultsToCSV(const std::vector<std::vector<TestResult>>& allResultsByGroup, 
                        const std::vector<std::string>& dataTypeNames,
                        const std::vector<size_t>& testSizes,
                        const std::vector<size_t>& patternSizes,
                        const std::string& fileName);

// Glavna funkcija test suite-a
void runTestSuite();

#endif // TESTFRAMEWORK_H