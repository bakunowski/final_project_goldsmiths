# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.14.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.14.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build

# Include any dependencies generated for this target.
include test/CMakeFiles/simplicial_cholesky_3.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/simplicial_cholesky_3.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/simplicial_cholesky_3.dir/flags.make

test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o: test/CMakeFiles/simplicial_cholesky_3.dir/flags.make
test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o: ../test/simplicial_cholesky.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o -c /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/test/simplicial_cholesky.cpp

test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.i"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/test/simplicial_cholesky.cpp > CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.i

test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.s"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/test/simplicial_cholesky.cpp -o CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.s

# Object files for target simplicial_cholesky_3
simplicial_cholesky_3_OBJECTS = \
"CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o"

# External object files for target simplicial_cholesky_3
simplicial_cholesky_3_EXTERNAL_OBJECTS =

test/simplicial_cholesky_3: test/CMakeFiles/simplicial_cholesky_3.dir/simplicial_cholesky.cpp.o
test/simplicial_cholesky_3: test/CMakeFiles/simplicial_cholesky_3.dir/build.make
test/simplicial_cholesky_3: test/CMakeFiles/simplicial_cholesky_3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable simplicial_cholesky_3"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simplicial_cholesky_3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/simplicial_cholesky_3.dir/build: test/simplicial_cholesky_3

.PHONY : test/CMakeFiles/simplicial_cholesky_3.dir/build

test/CMakeFiles/simplicial_cholesky_3.dir/clean:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test && $(CMAKE_COMMAND) -P CMakeFiles/simplicial_cholesky_3.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/simplicial_cholesky_3.dir/clean

test/CMakeFiles/simplicial_cholesky_3.dir/depend:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731 /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/test/CMakeFiles/simplicial_cholesky_3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/simplicial_cholesky_3.dir/depend

