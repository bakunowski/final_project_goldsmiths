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
include unsupported/test/CMakeFiles/autodiff_5.dir/depend.make

# Include the progress variables for this target.
include unsupported/test/CMakeFiles/autodiff_5.dir/progress.make

# Include the compile flags for this target's objects.
include unsupported/test/CMakeFiles/autodiff_5.dir/flags.make

unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.o: unsupported/test/CMakeFiles/autodiff_5.dir/flags.make
unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.o: ../unsupported/test/autodiff.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.o"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/autodiff_5.dir/autodiff.cpp.o -c /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/autodiff.cpp

unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/autodiff_5.dir/autodiff.cpp.i"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/autodiff.cpp > CMakeFiles/autodiff_5.dir/autodiff.cpp.i

unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/autodiff_5.dir/autodiff.cpp.s"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test/autodiff.cpp -o CMakeFiles/autodiff_5.dir/autodiff.cpp.s

# Object files for target autodiff_5
autodiff_5_OBJECTS = \
"CMakeFiles/autodiff_5.dir/autodiff.cpp.o"

# External object files for target autodiff_5
autodiff_5_EXTERNAL_OBJECTS =

unsupported/test/autodiff_5: unsupported/test/CMakeFiles/autodiff_5.dir/autodiff.cpp.o
unsupported/test/autodiff_5: unsupported/test/CMakeFiles/autodiff_5.dir/build.make
unsupported/test/autodiff_5: unsupported/test/CMakeFiles/autodiff_5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable autodiff_5"
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/autodiff_5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
unsupported/test/CMakeFiles/autodiff_5.dir/build: unsupported/test/autodiff_5

.PHONY : unsupported/test/CMakeFiles/autodiff_5.dir/build

unsupported/test/CMakeFiles/autodiff_5.dir/clean:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test && $(CMAKE_COMMAND) -P CMakeFiles/autodiff_5.dir/cmake_clean.cmake
.PHONY : unsupported/test/CMakeFiles/autodiff_5.dir/clean

unsupported/test/CMakeFiles/autodiff_5.dir/depend:
	cd /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731 /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/unsupported/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test /Users/bakunowski/Documents/goldsmiths/2018_19/dissertation/project/GranularSynth/eigen-eigen-323c052e1731/build/unsupported/test/CMakeFiles/autodiff_5.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : unsupported/test/CMakeFiles/autodiff_5.dir/depend

