#include <iostream>
#include "framework/TestFramework.h"

int main() {
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "   🔬 STRING MATCHING ALGORITHMS - UNIFIED PERFORMANCE + CACHE ANALYSIS 🔬    " << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    
    std::cout << "\n🧪 RESEARCH FRAMEWORK:" << std::endl;
    std::cout << "   • Naive (Brute Force) Algorithm" << std::endl;
    std::cout << "   • Knuth-Morris-Pratt (KMP) Algorithm" << std::endl;
    std::cout << "   • Rabin-Karp Algorithm" << std::endl;
    std::cout << "   • Boyer-Moore Algorithm" << std::endl;
    
    std::cout << "\n⚡ MEASUREMENT CAPABILITIES:" << std::endl;
    std::cout << "   Platform: ";
#ifdef _WIN32
    std::cout << "Windows (Process Memory API + Estimates)" << std::endl;
#elif defined(__linux__)
    std::cout << "Linux (perf_event Hardware Counters + /proc)" << std::endl;
#elif defined(__APPLE__)
    std::cout << "macOS (getrusage + Cache Estimates)" << std::endl;
#else
    std::cout << "Generic (Basic Timing + Memory Estimates)" << std::endl;
#endif

    std::cout << "\n📊 UNIFIED ANALYSIS FEATURES:" << std::endl;
    std::cout << "   ✅ Real-time Performance Measurement" << std::endl;
    std::cout << "   ✅ Hardware Cache Performance Counters" << std::endl;
    std::cout << "   ✅ Cache Boundary Effect Analysis" << std::endl;
    std::cout << "   ✅ Performance-Cache Correlation Statistics" << std::endl;
    std::cout << "   ✅ Efficiency Scoring System" << std::endl;
    std::cout << "   ✅ Academic-Ready Dataset Export" << std::endl;
    
    std::cout << "\n🎯 RESEARCH OUTPUTS:" << std::endl;
    std::cout << "   📈 unified_performance_cache_analysis.csv - Complete dataset" << std::endl;
    std::cout << "   📊 cache_performance_correlation.csv - Statistical correlations" << std::endl;
    std::cout << "   🔍 cache_boundary_effects.csv - Boundary degradation analysis" << std::endl;
    
    std::cout << "\n🚀 STARTING UNIFIED ANALYSIS..." << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    
    // Automatically run the unified analysis (recommended approach)
    runUnifiedAnalysis();
    
    std::cout << "\n🎓 RESEARCH COMPLETED!" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "📊 SUMMARY OF RESULTS:" << std::endl;
    std::cout << "   • Performance metrics across multiple data types" << std::endl;
    std::cout << "   • Cache behavior analysis at critical boundaries" << std::endl;
    std::cout << "   • Correlation analysis between cache misses and execution time" << std::endl;
    std::cout << "   • Efficiency scoring for algorithm comparison" << std::endl;
    std::cout << "   • Datasets ready for statistical analysis" << std::endl;
    
    std::cout << "\n📝 RESEARCH HYPOTHESIS TESTING:" << std::endl;
    std::cout << "   'Cache memory behavior significantly impacts string matching" << std::endl;
    std::cout << "    algorithm performance, with measurable correlation between" << std::endl;
    std::cout << "    cache miss rates and execution times across cache boundaries'" << std::endl;
    
    std::cout << "\n💡 NEXT STEPS:" << std::endl;
    std::cout << "   1. Analyze the generated CSV files with statistical software" << std::endl;
    std::cout << "   2. Look for correlation patterns in cache_performance_correlation.csv" << std::endl;
    std::cout << "   3. Examine performance degradation at cache boundaries" << std::endl;
    std::cout << "   4. Compare algorithm efficiency scores for academic publication" << std::endl;
    
    std::cout << "\n🔬 For additional analysis options, modify main.cpp to call runTestSuite()" << std::endl;
    std::cout << "   instead of runUnifiedAnalysis() for legacy test selection." << std::endl;
    
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}