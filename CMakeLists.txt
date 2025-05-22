cmake_minimum_required(VERSION 3.10)
project(StringMatchingAlgorithms)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
endif()

# Include directories
include_directories(src)

# Source files
set(ALGORITHM_SOURCES
    src/algorithms/Naive.cpp
    src/algorithms/KMP.cpp
    src/algorithms/RabinKarp.cpp
    src/algorithms/BoyerMoore.cpp
)

set(FRAMEWORK_SOURCES
    src/framework/TestFramework.cpp
    src/framework/TestUtils.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

# Create executable
add_executable(StringMatchingAlgorithms 
    ${MAIN_SOURCES}
    ${ALGORITHM_SOURCES}
    ${FRAMEWORK_SOURCES}
)

# Platform-specific libraries for memory measurement
if(WIN32)
    target_link_libraries(StringMatchingAlgorithms psapi)
elseif(UNIX AND NOT APPLE)
    # Linux - no additional libraries needed for /proc access
    target_compile_definitions(StringMatchingAlgorithms PRIVATE __linux__)
elseif(APPLE)
    # macOS - no additional libraries needed for getrusage
    target_compile_definitions(StringMatchingAlgorithms PRIVATE __APPLE__)
endif()

# Optional: Enable additional warnings and debugging info
option(ENABLE_DEBUG "Enable debug build" OFF)
if(ENABLE_DEBUG)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /DDEBUG")
    endif()
endif()

# Optional: Enable profiling
option(ENABLE_PROFILING "Enable profiling support" OFF)
if(ENABLE_PROFILING)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    endif()
endif()

# Optional: Enable sanitizers for debugging
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
    endif()
endif()

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Install target (optional)
install(TARGETS StringMatchingAlgorithms DESTINATION bin)

# Custom targets for convenience
add_custom_target(run
    COMMAND StringMatchingAlgorithms
    DEPENDS StringMatchingAlgorithms
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running String Matching Algorithms test suite"
)

add_custom_target(clean-results
    COMMAND ${CMAKE_COMMAND} -E remove -f rezultati_testiranja*.csv
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning result files"
)

# Testing with CTest (optional advanced feature)
enable_testing()

add_test(NAME BasicAlgorithmTest 
         COMMAND StringMatchingAlgorithms)

# Set test properties
set_tests_properties(BasicAlgorithmTest PROPERTIES 
                     TIMEOUT 300
                     PASS_REGULAR_EXPRESSION "Testiranje završeno")

# Generate compile_commands.json for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)