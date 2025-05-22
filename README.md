# String Matching Algorithms

This project implements and compares four classic string matching algorithms with **real-time performance measurement** and **cross-platform memory profiling**:
- Naive (Brute Force) algorithm
- Knuth-Morris-Pratt (KMP) algorithm
- Rabin-Karp algorithm
- Boyer-Moore algorithm

## Project Overview

This implementation is part of a comprehensive thesis on the efficiency of string matching algorithms in various real-world scenarios. The algorithms are tested on four different types of data (plain text, system logs, network packets, and binary patterns) with varying text sizes and pattern lengths to provide an accurate performance analysis.

## Key Features

✅ **Real Memory Measurement**: Platform-specific memory profiling (Windows/Linux/macOS)  
✅ **Accurate Time Measurement**: Average of 3 runs for statistical accuracy  
✅ **Binary Data Support**: Handles binary network packets and malicious patterns  
✅ **Cross-Platform**: Tested on Windows, Linux, and macOS  
✅ **CSV Export**: Detailed results exported for further analysis  
✅ **Error Handling**: Robust exception handling prevents crashes  
✅ **Consistency Verification**: Validates algorithm correctness across all implementations  

## Algorithms

1. **Naive Algorithm**: A straightforward approach that checks every position in the text for a match with the pattern.
2. **Knuth-Morris-Pratt (KMP)**: Utilizes a pattern preprocessing step to avoid unnecessary comparisons.
3. **Rabin-Karp**: Uses hashing to find pattern matches, particularly effective for short to medium patterns.
4. **Boyer-Moore**: Employs bad character heuristic to skip portions of the text, often resulting in sub-linear time complexity.

## Project Structure

```
StringMatchingAlgorithms/
├── src/
│   ├── algorithms/
│   │   ├── Algorithms.h     # Header with algorithm declarations
│   │   ├── Naive.cpp        # Naive algorithm implementation
│   │   ├── KMP.cpp          # KMP algorithm implementation
│   │   ├── RabinKarp.cpp    # Rabin-Karp algorithm implementation
│   │   └── BoyerMoore.cpp   # Boyer-Moore algorithm implementation (fixed for binary data)
│   ├── framework/
│   │   ├── TestFramework.h  # Enhanced test framework header
│   │   └── TestFramework.cpp# Test framework with real memory measurement
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

The enhanced testing framework evaluates each algorithm based on:

1. **Execution Time**: Measured in milliseconds (average of 3 runs for accuracy)
2. **Accuracy**: Verifies that all algorithms find identical matches
3. **Memory Usage**: **Real memory consumption** measured using platform-specific APIs:
   - **Windows**: Process Memory API (`GetProcessMemoryInfo`)
   - **Linux**: `/proc/self/statm` filesystem
   - **macOS**: `getrusage()` system call

### Test Parameters:
- **Text sizes**: 1,000, 10,000, and 100,000 characters
- **Pattern lengths**: 3, 10, and 20 characters
- **Data types**:
  - **Clean text**: Alphanumeric characters + spaces
  - **System logs**: Realistic log entry formats with timestamps
  - **Network packets**: Simulated binary network data
  - **Binary patterns**: Malicious binary patterns with random bytes

## Performance Results

Based on comprehensive testing across all data types and sizes:

### Overall Performance Ranking (100,000 characters):

| Algorithm    | 3-char pattern | 10-char pattern | 20-char pattern | Binary Data Safe |
|--------------|----------------|-----------------|-----------------|------------------|
| **Boyer-Moore** | 1.1-1.3 ms    | 0.3-0.5 ms     | 0.16-0.3 ms    | ✅ |
| **Rabin-Karp** | 1.9-2.4 ms    | 1.9-2.0 ms     | 1.8-2.0 ms     | ✅ |
| **Naive**    | 2.0-3.3 ms     | 2.0-2.2 ms     | 1.9-2.0 ms     | ✅ |
| **KMP**      | 3.7-5.2 ms     | 3.6-3.9 ms     | 3.6-3.7 ms     | ✅ |

### Key Performance Insights:

1. **Boyer-Moore dominates**: Especially effective for longer patterns (20+ characters)
   - **Best case**: 0.16ms for 20-character patterns
   - **Scalability**: Performance improves with longer patterns
   
2. **Rabin-Karp consistency**: Stable performance across all pattern sizes
   - **Reliable**: ~2ms regardless of pattern length
   - **Predictable**: Good choice for varied pattern lengths
   
3. **Naive surprises**: Outperforms KMP in most scenarios
   - **Simple efficiency**: Cache-friendly memory access patterns
   - **Competitive**: Close to Rabin-Karp performance
   
4. **KMP limitations**: Slowest across all test cases
   - **Overhead**: LPS preprocessing overhead not compensated
   - **Use case**: Better for very long texts with repetitive patterns

### Memory Usage:
- **Boyer-Moore**: 1KB (bad character table)
- **Rabin-Karp**: 2KB (hash values)
- **Naive**: 1KB (minimal overhead)
- **KMP**: 0.01-0.08KB (LPS array size depends on pattern)

## Results Export

The test results are automatically exported to a CSV file (`rezultati_poboljsani.csv`) containing:
- Data type (Clean text, System logs, Network packets, Binary patterns)
- Text size (1K, 10K, 100K characters)
- Pattern length (3, 10, 20 characters)
- Algorithm name
- Execution time (ms)
- Number of matches found
- Memory usage (KB)
- Accuracy verification

### Example CSV Export:
```csv
Tip podataka,Veličina teksta,Dužina uzorka,Algoritam,Vrijeme (ms),Podudaranja,Memorija (KB),Tačnost
Čisti tekst,100000,20,Boyer-Moore,0.193,1,1.00,Da
Čisti tekst,100000,20,Rabin-Karp,1.885,1,2.00,Da
```

## Error Handling & Robustness

The framework includes comprehensive error handling:
- **Exception safety**: All algorithms wrapped in try-catch blocks
- **Memory corruption detection**: Bounds checking for binary data
- **Consistency verification**: Cross-validation between algorithms
- **Graceful degradation**: Program continues if one algorithm fails

## Platform Support

Tested and verified on:
- ✅ **macOS** (ARM64/Intel) - using `getrusage()`
- ✅ **Linux** (Ubuntu/CentOS) - using `/proc/self/statm`
- ✅ **Windows** (10/11) - using Process Memory API

## Recommendations

### For Production Use:
1. **Short patterns (3-10 chars)**: Use **Rabin-Karp** for consistency
2. **Long patterns (20+ chars)**: Use **Boyer-Moore** for maximum performance
3. **Binary data**: All algorithms are safe, but **Boyer-Moore** is fastest
4. **Unknown pattern length**: Use **Rabin-Karp** for predictable performance

### For Educational Purposes:
1. **Start with Naive** to understand the basic concept
2. **Study Boyer-Moore** to see advanced optimization techniques
3. **Compare results** using this framework to verify your implementations

## Contributing

Feel free to submit issues or pull requests to improve the algorithms or testing framework.

## License

This project is part of academic research. Please cite appropriately if used in academic work.

---

**Note**: All performance measurements were conducted on macOS with Apple Silicon. Results may vary on different platforms, but relative performance should remain consistent.