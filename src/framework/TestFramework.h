#pragma once
#include <string>
#include <vector>
#include <functional>

// Tipovi testnih podataka
enum TestDataType {
    CLEAN_TEXT,
    SYSTEM_LOGS,
    NETWORK_PACKETS,
    BINARY_PATTERNS
};

// Struktura za rezultate testiranja
struct TestResult {
    std::string algorithmName;
    double executionTimeMs;
    int matchesFound;
    double memoryUsageKB;
    bool isCorrect;
    std::string dataTypeName;
    size_t textSize;
    size_t patternSize;
};

// Generatori testnih podataka
std::string generateTestData(TestDataType type, size_t length);
std::string generatePattern(const std::string& text, size_t patternLength, bool ensureMatch = true);

// Funkcije za testiranje i mjerenje
TestResult runTest(const std::string& algorithmName, 
                  std::vector<int> (*algorithm)(const std::string&, const std::string&),
                  const std::string& text, 
                  const std::string& pattern,
                  const std::vector<int>& expectedMatches);

void runTestSuite();
void displayResults(const std::vector<TestResult>& results);