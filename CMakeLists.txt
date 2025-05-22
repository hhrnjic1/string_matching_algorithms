cmake_minimum_required(VERSION 3.10)
project(StringMatchingAlgorithms)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Platform-specific libraries for memory measurement
if(WIN32)
    target_link_libraries(StringMatchingAlgorithms psapi)
endif()

# Print build info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Custom target to run the program
add_custom_target(run
    COMMAND StringMatchingAlgorithms
    DEPENDS StringMatchingAlgorithms
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running String Matching Algorithms test suite"
)