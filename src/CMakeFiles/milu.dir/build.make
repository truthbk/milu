# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/truth/Development/github/milu

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/truth/Development/github/milu

# Include any dependencies generated for this target.
include src/CMakeFiles/milu.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/milu.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/milu.dir/flags.make

src/CMakeFiles/milu.dir/milu/milu.c.o: src/CMakeFiles/milu.dir/flags.make
src/CMakeFiles/milu.dir/milu/milu.c.o: src/milu/milu.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/truth/Development/github/milu/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/CMakeFiles/milu.dir/milu/milu.c.o"
	cd /home/truth/Development/github/milu/src && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/milu.dir/milu/milu.c.o   -c /home/truth/Development/github/milu/src/milu/milu.c

src/CMakeFiles/milu.dir/milu/milu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/milu.dir/milu/milu.c.i"
	cd /home/truth/Development/github/milu/src && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/truth/Development/github/milu/src/milu/milu.c > CMakeFiles/milu.dir/milu/milu.c.i

src/CMakeFiles/milu.dir/milu/milu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/milu.dir/milu/milu.c.s"
	cd /home/truth/Development/github/milu/src && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/truth/Development/github/milu/src/milu/milu.c -o CMakeFiles/milu.dir/milu/milu.c.s

src/CMakeFiles/milu.dir/milu/milu.c.o.requires:
.PHONY : src/CMakeFiles/milu.dir/milu/milu.c.o.requires

src/CMakeFiles/milu.dir/milu/milu.c.o.provides: src/CMakeFiles/milu.dir/milu/milu.c.o.requires
	$(MAKE) -f src/CMakeFiles/milu.dir/build.make src/CMakeFiles/milu.dir/milu/milu.c.o.provides.build
.PHONY : src/CMakeFiles/milu.dir/milu/milu.c.o.provides

src/CMakeFiles/milu.dir/milu/milu.c.o.provides.build: src/CMakeFiles/milu.dir/milu/milu.c.o

# Object files for target milu
milu_OBJECTS = \
"CMakeFiles/milu.dir/milu/milu.c.o"

# External object files for target milu
milu_EXTERNAL_OBJECTS =

src/libmilu.a: src/CMakeFiles/milu.dir/milu/milu.c.o
src/libmilu.a: src/CMakeFiles/milu.dir/build.make
src/libmilu.a: src/CMakeFiles/milu.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libmilu.a"
	cd /home/truth/Development/github/milu/src && $(CMAKE_COMMAND) -P CMakeFiles/milu.dir/cmake_clean_target.cmake
	cd /home/truth/Development/github/milu/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/milu.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/milu.dir/build: src/libmilu.a
.PHONY : src/CMakeFiles/milu.dir/build

src/CMakeFiles/milu.dir/requires: src/CMakeFiles/milu.dir/milu/milu.c.o.requires
.PHONY : src/CMakeFiles/milu.dir/requires

src/CMakeFiles/milu.dir/clean:
	cd /home/truth/Development/github/milu/src && $(CMAKE_COMMAND) -P CMakeFiles/milu.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/milu.dir/clean

src/CMakeFiles/milu.dir/depend:
	cd /home/truth/Development/github/milu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/truth/Development/github/milu /home/truth/Development/github/milu/src /home/truth/Development/github/milu /home/truth/Development/github/milu/src /home/truth/Development/github/milu/src/CMakeFiles/milu.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/milu.dir/depend
