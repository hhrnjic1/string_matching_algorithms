#include <iostream>
#include "framework/TestFramework.h"

int main() {
    std::cout << "Poređenje algoritama za podudaranje stringova" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    runTestSuite();
    
    std::cout << "\nTestiranje završeno. Pritisnite Enter za izlaz..." << std::endl;
    std::cin.get();
    
    return 0;
}