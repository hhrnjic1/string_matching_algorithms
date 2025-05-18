cmake_minimum_required(VERSION 3.10)
project(StringMatchingAlgorithms)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Uključi fajlove
include_directories(src)

# Dodaj sve izvorne fajlove
add_executable(StringMatchingAlgorithms 
    src/main.cpp 
    src/algorithms/Naive.cpp
    src/algorithms/KMP.cpp
    src/algorithms/RabinKarp.cpp
    src/algorithms/BoyerMoore.cpp
    src/framework/TestFramework.cpp
)