#include <iostream>
#include "framework/TestFramework.h"

int main() {
    std::cout << "=====================================================" << std::endl;
    std::cout << "   POREĐENJE ALGORITAMA ZA PODUDARANJE STRINGOVA    " << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << "Implementirani algoritmi:" << std::endl;
    std::cout << "• Naivni (Brute Force) algoritam" << std::endl;
    std::cout << "• Knuth-Morris-Pratt (KMP) algoritam" << std::endl;
    std::cout << "• Rabin-Karp algoritam" << std::endl;
    std::cout << "• Boyer-Moore algoritam" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    // Provjeri platformu za mjerenje memorije
    std::cout << "Sistem za mjerenje memorije: ";
#ifdef _WIN32
    std::cout << "Windows (Process Memory API)" << std::endl;
#elif defined(__linux__)
    std::cout << "Linux (/proc filesystem)" << std::endl;
#elif defined(__APPLE__)
    std::cout << "macOS (getrusage)" << std::endl;
#else
    std::cout << "Nepoznat (koristit će se procjene)" << std::endl;
#endif
    
    std::cout << "\nPokretanje testova..." << std::endl;
    
    // Pokreni glavni test suite
    runTestSuite();
    
    std::cout << "\nTestiranje završeno. Pritisnite Enter za izlaz..." << std::endl;
    std::cin.get();
    
    return 0;
}