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
include unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/depend.make

# Include the progress variables for this target.
include unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/progress.make

# Include the compile flags for this target's objects.
include unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/flags.make

unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o: unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/flags.make
unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o: ../unsupported/test/cxx11_tensor_assign.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o -c /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/cxx11_tensor_assign.cpp

unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.i"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/cxx11_tensor_assign.cpp > CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.i

unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.s"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/cxx11_tensor_assign.cpp -o CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.s

# Object files for target cxx11_tensor_assign
cxx11_tensor_assign_OBJECTS = \
"CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o"

# External object files for target cxx11_tensor_assign
cxx11_tensor_assign_EXTERNAL_OBJECTS =

unsupported/test/cxx11_tensor_assign: unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/cxx11_tensor_assign.cpp.o
unsupported/test/cxx11_tensor_assign: unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/build.make
unsupported/test/cxx11_tensor_assign: unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cxx11_tensor_assign"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cxx11_tensor_assign.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/build: unsupported/test/cxx11_tensor_assign

.PHONY : unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/build

unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/clean:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && $(CMAKE_COMMAND) -P CMakeFiles/cxx11_tensor_assign.dir/cmake_clean.cmake
.PHONY : unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/clean

unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/depend:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731 /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : unsupported/test/CMakeFiles/cxx11_tensor_assign.dir/depend

