#include <iostream>
#include <string>
#include <vector>
#include "framework/TestFramework.h"

using namespace std;

void showMenu() {
    cout << "\n=== MENI TESTIRANJA ALGORITAMA ===" << endl;
    cout << "1. Pokreni standardni test suite" << endl;
    cout << "2. Pokreni brzi test (samo manje veličine)" << endl;
    cout << "3. Pokreni samo stress testove" << endl;
    cout << "4. Pokreni custom benchmark" << endl;
    cout << "5. Pokreni edge case testove" << endl;
    cout << "6. Pokreni pattern-specifične testove" << endl;
    cout << "7. Izađi" << endl;
    cout << "Izbor: ";
}

void runQuickTest() {
    cout << "\n=== BRZI TEST ===" << endl;
    
    TestConfiguration config;
    config.textSizes = {1000, 5000};  // Manje veličine
    config.patternSizes = {3, 10};    // Kraći pattern-i
    config.enableStressTests = false;
    config.numIterations = 3;         // Manje iteracija
    config.categories = {BASIC_TESTS, EDGE_CASE_TESTS};
    config.outputFileName = "brzi_test_rezultati.csv";
    
    runCustomTestSuite(config);
}

void runStressOnly() {
    cout << "\n=== SAMO STRESS TESTOVI ===" << endl;
    
    TestConfiguration config;
    config.categories = {STRESS_TESTS};
    config.enableStressTests = true;
    config.outputFileName = "stress_test_rezultati.csv";
    
    // Dodaj većje veličine za stress test
    config.textSizes = {100000, 500000, 1000000};
    config.patternSizes = {10, 50, 100};
    
    runCustomTestSuite(config);
}

void runCustomBenchmark() {
    cout << "\n=== CUSTOM BENCHMARK ===" << endl;
    cout << "Ovo je detaljni benchmark svih algoritama na istom tekstu.\n" << endl;
    
    // Generiši test podatke
    string text = generateTestData(CLEAN_TEXT, 50000);
    string pattern = generatePattern(text, 15, true);
    
    cout << "Generisani tekst: " << text.length() << " karaktera" << endl;
    cout << "Pattern za pretragu: \"" << pattern << "\" (dužina: " << pattern.length() << ")" << endl;
    
    // Pokreni benchmark za svaki algoritam
    TestUtils::runBenchmark("Naivni algoritam", naiveSearch, text, pattern, 50);
    TestUtils::runBenchmark("KMP algoritam", kmpSearch, text, pattern, 50);
    TestUtils::runBenchmark("Rabin-Karp algoritam", rabinKarpSearch, text, pattern, 50);
    TestUtils::runBenchmark("Boyer-Moore algoritam", boyerMooreSearch, text, pattern, 50);
}

void runEdgeCaseOnly() {
    cout << "\n=== SAMO EDGE CASE TESTOVI ===" << endl;
    
    vector<TestResult> results = runEdgeCaseTests();
    displayResults(results);
    
    // Dodatni edge case testovi
    cout << "\nDodatni edge case testovi:" << endl;
    
    // Test sa vrlo dugim pattern-om
    string longText = generateTestData(CLEAN_TEXT, 100);
    string longPattern = generateTestData(CLEAN_TEXT, 150); // Duži od teksta
    
    cout << "Test: Pattern duži od teksta" << endl;
    cout << "Tekst: " << longText.length() << " karaktera" << endl;
    cout << "Pattern: " << longPattern.length() << " karaktera" << endl;
    
    vector<TestResult> longPatternResults;
    longPatternResults.push_back(runTest("Naivni (dugačak pattern)", naiveSearch, longText, longPattern, {}));
    longPatternResults.push_back(runTest("KMP (dugačak pattern)", kmpSearch, longText, longPattern, {}));
    
    displayResults(longPatternResults);
}

void runPatternTests() {
    cout << "\n=== PATTERN-SPECIFIČNI TESTOVI ===" << endl;
    
    TestConfiguration config;
    config.categories = {PATTERN_TESTS};
    config.outputFileName = "pattern_test_rezultati.csv";
    
    runCustomTestSuite(config);
}

int main() {
    cout << "=====================================================" << endl;
    cout << "   POREĐENJE ALGORITAMA ZA PODUDARANJE STRINGOVA    " << endl;
    cout << "=====================================================" << endl;
    cout << "Implementirani algoritmi:" << endl;
    cout << "• Naivni (Brute Force) algoritam" << endl;
    cout << "• Knuth-Morris-Pratt (KMP) algoritam" << endl;
    cout << "• Rabin-Karp algoritam" << endl;
    cout << "• Boyer-Moore algoritam" << endl;
    cout << "=====================================================" << endl;
    
    // Provjeri platformu za mjerenje memorije
    cout << "Sistem za mjerenje memorije: ";
#ifdef _WIN32
    cout << "Windows (Process Memory API)" << endl;
#elif defined(__linux__)
    cout << "Linux (/proc filesystem)" << endl;
#elif defined(__APPLE__)
    cout << "macOS (getrusage)" << endl;
#else
    cout << "Nepoznat (koristit će se procjene)" << endl;
#endif
    
    int choice;
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "\nPokretanje standardnog test suite-a..." << endl;
                runTestSuite();
                break;
                
            case 2:
                runQuickTest();
                break;
                
            case 3:
                runStressOnly();
                break;
                
            case 4:
                runCustomBenchmark();
                break;
                
            case 5:
                runEdgeCaseOnly();
                break;
                
            case 6:
                runPatternTests();
                break;
                
            case 7:
                cout << "Izlaz iz programa." << endl;
                break;
                
            default:
                cout << "Nepoznat izbor. Pokušajte ponovo." << endl;
                break;
        }
        
        if (choice != 7) {
            cout << "\nPritisnite Enter za povratak na meni...";
            cin.ignore();
            cin.get();
        }
        
    } while (choice != 7);
    
    cout << "\nTestiranje završeno. Hvala što ste koristili String Matching Test Suite!" << endl;
    
    return 0;
}