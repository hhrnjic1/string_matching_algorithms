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
    
    // Enhanced platform info
    std::cout << "Sistem za mjerenje memorije: ";
#ifdef _WIN32
    std::cout << "Windows (Process Memory API)" << std::endl;
#elif defined(__linux__)
    std::cout << "Linux (/proc filesystem + perf counters)" << std::endl;
#elif defined(__APPLE__)
    std::cout << "macOS (getrusage + cache estimates)" << std::endl;
#else
    std::cout << "Nepoznat (osnovne procjene)" << std::endl;
#endif

    std::cout << "\n🧪 EXPERIMENTAL FEATURES:" << std::endl;
    std::cout << "✅ Real memory measurement" << std::endl;
    std::cout << "✅ Cache performance analysis" << std::endl;
    std::cout << "✅ Hardware counter integration" << std::endl;
    std::cout << "✅ Academic research support" << std::endl;
    
    std::cout << "\nPokretanje enhanced test suite-a..." << std::endl;
    
    // Run the enhanced test suite with cache experiment options
    runTestSuite();
    
    std::cout << "\n🎓 ACADEMIC NOTES:" << std::endl;
    std::cout << "• Cache experiment data perfect for research papers" << std::endl;
    std::cout << "• CSV exports ready for statistical analysis" << std::endl;
    std::cout << "• Hardware counters provide real performance insights" << std::endl;
    
    std::cout << "\nTestiranje završeno. Pritisnite Enter za izlaz..." << std::endl;
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}