# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tom/hdtree/main/cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tom/hdtree/main/cpp/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/hdtree-eg-save.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/CMakeFiles/hdtree-eg-save.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/hdtree-eg-save.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/hdtree-eg-save.dir/flags.make

examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o: examples/CMakeFiles/hdtree-eg-save.dir/flags.make
examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o: ../examples/save.cxx
examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o: examples/CMakeFiles/hdtree-eg-save.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tom/hdtree/main/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o"
	cd /home/tom/hdtree/main/cpp/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o -MF CMakeFiles/hdtree-eg-save.dir/save.cxx.o.d -o CMakeFiles/hdtree-eg-save.dir/save.cxx.o -c /home/tom/hdtree/main/cpp/examples/save.cxx

examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hdtree-eg-save.dir/save.cxx.i"
	cd /home/tom/hdtree/main/cpp/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tom/hdtree/main/cpp/examples/save.cxx > CMakeFiles/hdtree-eg-save.dir/save.cxx.i

examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hdtree-eg-save.dir/save.cxx.s"
	cd /home/tom/hdtree/main/cpp/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tom/hdtree/main/cpp/examples/save.cxx -o CMakeFiles/hdtree-eg-save.dir/save.cxx.s

# Object files for target hdtree-eg-save
hdtree__eg__save_OBJECTS = \
"CMakeFiles/hdtree-eg-save.dir/save.cxx.o"

# External object files for target hdtree-eg-save
hdtree__eg__save_EXTERNAL_OBJECTS =

examples/hdtree-eg-save: examples/CMakeFiles/hdtree-eg-save.dir/save.cxx.o
examples/hdtree-eg-save: examples/CMakeFiles/hdtree-eg-save.dir/build.make
examples/hdtree-eg-save: libHDTree.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libcrypto.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libcurl.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libpthread.a
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libsz.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libz.so
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libdl.a
examples/hdtree-eg-save: /usr/lib/x86_64-linux-gnu/libm.so
examples/hdtree-eg-save: examples/CMakeFiles/hdtree-eg-save.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tom/hdtree/main/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hdtree-eg-save"
	cd /home/tom/hdtree/main/cpp/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hdtree-eg-save.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/hdtree-eg-save.dir/build: examples/hdtree-eg-save
.PHONY : examples/CMakeFiles/hdtree-eg-save.dir/build

examples/CMakeFiles/hdtree-eg-save.dir/clean:
	cd /home/tom/hdtree/main/cpp/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/hdtree-eg-save.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/hdtree-eg-save.dir/clean

examples/CMakeFiles/hdtree-eg-save.dir/depend:
	cd /home/tom/hdtree/main/cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tom/hdtree/main/cpp /home/tom/hdtree/main/cpp/examples /home/tom/hdtree/main/cpp/build /home/tom/hdtree/main/cpp/build/examples /home/tom/hdtree/main/cpp/build/examples/CMakeFiles/hdtree-eg-save.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/hdtree-eg-save.dir/depend

