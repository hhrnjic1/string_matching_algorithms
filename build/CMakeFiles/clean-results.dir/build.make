# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/hamza/Desktop/StringMatchingAlgorithms

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/hamza/Desktop/StringMatchingAlgorithms/build

# Utility rule file for clean-results.

# Include any custom commands dependencies for this target.
include CMakeFiles/clean-results.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/clean-results.dir/progress.make

CMakeFiles/clean-results:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/hamza/Desktop/StringMatchingAlgorithms/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Cleaning all result files"
	/opt/homebrew/bin/cmake -E remove -f rezultati_*.csv cache_experiment_*.csv

CMakeFiles/clean-results.dir/codegen:
.PHONY : CMakeFiles/clean-results.dir/codegen

clean-results: CMakeFiles/clean-results
clean-results: CMakeFiles/clean-results.dir/build.make
.PHONY : clean-results

# Rule to build all files generated by this target.
CMakeFiles/clean-results.dir/build: clean-results
.PHONY : CMakeFiles/clean-results.dir/build

CMakeFiles/clean-results.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/clean-results.dir/cmake_clean.cmake
.PHONY : CMakeFiles/clean-results.dir/clean

CMakeFiles/clean-results.dir/depend:
	cd /Users/hamza/Desktop/StringMatchingAlgorithms/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/hamza/Desktop/StringMatchingAlgorithms /Users/hamza/Desktop/StringMatchingAlgorithms /Users/hamza/Desktop/StringMatchingAlgorithms/build /Users/hamza/Desktop/StringMatchingAlgorithms/build /Users/hamza/Desktop/StringMatchingAlgorithms/build/CMakeFiles/clean-results.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/clean-results.dir/depend

