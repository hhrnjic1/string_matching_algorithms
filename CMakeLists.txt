cmake_minimum_required(VERSION 3.10)
project(StringMatchingAlgorithms)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags for performance
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W4")
endif()

# Include directories
include_directories(src)

# Create executable
add_executable(StringMatchingAlgorithms 
    src/main.cpp 
    src/algorithms/Naive.cpp
    src/algorithms/KMP.cpp
    src/algorithms/RabinKarp.cpp
    src/algorithms/BoyerMoore.cpp
    src/framework/TestFramework.cpp
)

# Platform-specific libraries for memory and performance measurement
if(WIN32)
    target_link_libraries(StringMatchingAlgorithms psapi)
    target_compile_definitions(StringMatchingAlgorithms PRIVATE _WIN32)
elseif(UNIX AND NOT APPLE)
    # Linux - link with rt for performance counters if available
    find_library(RT_LIB rt)
    if(RT_LIB)
        target_link_libraries(StringMatchingAlgorithms ${RT_LIB})
    endif()
    target_compile_definitions(StringMatchingAlgorithms PRIVATE __linux__)
elseif(APPLE)
    # macOS specific settings
    target_compile_definitions(StringMatchingAlgorithms PRIVATE __APPLE__)
endif()

# Enhanced build options for research
option(ENABLE_CACHE_PROFILING "Enable detailed cache profiling (Linux only)" OFF)
if(ENABLE_CACHE_PROFILING AND UNIX AND NOT APPLE)
    target_compile_definitions(StringMatchingAlgorithms PRIVATE ENABLE_DETAILED_PROFILING)
    message(STATUS "Cache profiling enabled - requires sudo privileges for perf_event")
endif()

# Print enhanced build info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Cache profiling: ${ENABLE_CACHE_PROFILING}")

# Enhanced custom targets
add_custom_target(run
    COMMAND StringMatchingAlgorithms
    DEPENDS StringMatchingAlgorithms
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Enhanced String Matching Algorithms test suite"
)

add_custom_target(run-cache-experiment
    COMMAND StringMatchingAlgorithms
    DEPENDS StringMatchingAlgorithms
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Cache Performance Experiment"
)

add_custom_target(clean-results
    COMMAND ${CMAKE_COMMAND} -E remove -f rezultati_*.csv cache_experiment_*.csv
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning all result files"
)

# Academic analysis target
add_custom_target(analyze-results
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/analyze_results.py ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS StringMatchingAlgorithms
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running academic analysis on results (requires Python3)"
)

# Testing with CTest for academic validation
enable_testing()

add_test(NAME BasicAlgorithmTest 
         COMMAND StringMatchingAlgorithms)

add_test(NAME CacheExperimentTest
         COMMAND StringMatchingAlgorithms)

# Set test properties
set_tests_properties(BasicAlgorithmTest PROPERTIES 
                     TIMEOUT 300
                     PASS_REGULAR_EXPRESSION "Testiranje završeno")

set_tests_properties(CacheExperimentTest PROPERTIES 
                     TIMEOUT 600
                     PASS_REGULAR_EXPRESSION "CACHE EXPERIMENT COMPLETED")

# Generate compile_commands.json for development
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)