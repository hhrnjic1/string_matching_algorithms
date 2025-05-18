# String Matching Algorithms

This project implements and compares four classic string matching algorithms:
- Naive (Brute Force) algorithm
- Knuth-Morris-Pratt (KMP) algorithm
- Rabin-Karp algorithm
- Boyer-Moore algorithm

## Project Overview

This implementation is part of a thesis on the efficiency of string matching algorithms in various scenarios. The algorithms are tested on four different types of data (plain text, system logs, network packets, and binary patterns) with varying text sizes and pattern lengths to provide a comprehensive performance analysis.

## Algorithms

1. **Naive Algorithm**: A straightforward approach that checks every position in the text for a match with the pattern.
2. **Knuth-Morris-Pratt (KMP)**: Utilizes a pattern preprocessing step to avoid unnecessary comparisons.
3. **Rabin-Karp**: Uses hashing to find pattern matches, making it particularly effective for multiple pattern searching.
4. **Boyer-Moore**: Employs two heuristics (bad character and good suffix) to skip portions of the text, often resulting in sub-linear time complexity in practice.

## Project Structure

```
StringMatchingAlgorithms/
├── src/
│   ├── algorithms/
│   │   ├── Algorithms.h     # Header with algorithm declarations
│   │   ├── Naive.cpp        # Naive algorithm implementation
│   │   ├── KMP.cpp          # KMP algorithm implementation
│   │   ├── RabinKarp.cpp    # Rabin-Karp algorithm implementation
│   │   └── BoyerMoore.cpp   # Boyer-Moore algorithm implementation
│   ├── framework/
│   │   ├── TestFramework.h  # Test framework header
│   │   └── TestFramework.cpp# Test framework implementation
│   └── main.cpp             # Main program entry point
├── .vscode/                 # VS Code configuration
│   ├── tasks.json
│   └── launch.json
└── CMakeLists.txt           # CMake build configuration
```

## Requirements

- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake (version 3.10 or higher)
- (Optional) Visual Studio Code with C/C++ extension for development

## Building and Running

### Using Command Line with CMake

1. Clone the repository:
   ```bash
   git clone https://github.com/hhrnjic1/string_matching_algorithms.git
   cd string_matching_algorithms
   ```

2. Create a build directory and run CMake:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

3. Run the program:
   ```bash
   ./build/StringMatchingAlgorithms
   ```

### Using Visual Studio Code

1. Open the project folder in VS Code
2. Press F1 and type "CMake: Configure", then press Enter
3. After configuration, press F1 and type "CMake: Build", then press Enter
4. To run the program, press F5 or use the "Run" task

## Test Methodology

The testing framework evaluates each algorithm based on:

1. **Execution Time**: Measured in milliseconds for various input sizes
2. **Accuracy**: Whether the algorithm correctly finds all pattern occurrences
3. **Memory Usage**: Approximate memory consumption of each algorithm

Test parameters:
- **Text sizes**: 1,000, 10,000, and 100,000 characters
- **Pattern lengths**: 3, 10, and 20 characters
- **Data types**:
  - Clean text (alphanumeric characters)
  - System logs (typical log entry formats)
  - Network packets (simulated binary network data)
  - Binary patterns (simulated malicious binary patterns)

## Results

The test results are displayed in the terminal during execution and are also exported to a CSV file (`rezultati_testiranja.csv`) for further analysis. The CSV file contains the following columns:
- Data type
- Text size
- Pattern length
- Algorithm name
- Execution time (ms)
- Number of matches found
- Memory usage (KB)
- Accuracy

## Performance Summary

From the test results, we can observe that:

1. **Boyer-Moore** algorithm generally performs best, especially for longer patterns
2. **Rabin-Karp** is effective for multiple pattern matching scenarios
3. **KMP** offers stable performance regardless of pattern distribution
4. **Naive** algorithm's performance degrades significantly with larger texts

## License

This project is available under the MIT License. See the LICENSE file for details.