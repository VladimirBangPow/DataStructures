# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_SOURCE_DIR = /Users/hassanamad/Projects/DataStructures

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/hassanamad/Projects/DataStructures/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/test_stack.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/test_stack.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test_stack.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test_stack.dir/flags.make

tests/CMakeFiles/test_stack.dir/codegen:
.PHONY : tests/CMakeFiles/test_stack.dir/codegen

tests/CMakeFiles/test_stack.dir/test_stack.c.o: tests/CMakeFiles/test_stack.dir/flags.make
tests/CMakeFiles/test_stack.dir/test_stack.c.o: /Users/hassanamad/Projects/DataStructures/tests/test_stack.c
tests/CMakeFiles/test_stack.dir/test_stack.c.o: tests/CMakeFiles/test_stack.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/hassanamad/Projects/DataStructures/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/test_stack.dir/test_stack.c.o"
	cd /Users/hassanamad/Projects/DataStructures/build/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT tests/CMakeFiles/test_stack.dir/test_stack.c.o -MF CMakeFiles/test_stack.dir/test_stack.c.o.d -o CMakeFiles/test_stack.dir/test_stack.c.o -c /Users/hassanamad/Projects/DataStructures/tests/test_stack.c

tests/CMakeFiles/test_stack.dir/test_stack.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test_stack.dir/test_stack.c.i"
	cd /Users/hassanamad/Projects/DataStructures/build/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/hassanamad/Projects/DataStructures/tests/test_stack.c > CMakeFiles/test_stack.dir/test_stack.c.i

tests/CMakeFiles/test_stack.dir/test_stack.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test_stack.dir/test_stack.c.s"
	cd /Users/hassanamad/Projects/DataStructures/build/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/hassanamad/Projects/DataStructures/tests/test_stack.c -o CMakeFiles/test_stack.dir/test_stack.c.s

# Object files for target test_stack
test_stack_OBJECTS = \
"CMakeFiles/test_stack.dir/test_stack.c.o"

# External object files for target test_stack
test_stack_EXTERNAL_OBJECTS =

tests/test_stack: tests/CMakeFiles/test_stack.dir/test_stack.c.o
tests/test_stack: tests/CMakeFiles/test_stack.dir/build.make
tests/test_stack: libMyDataStructures.a
tests/test_stack: tests/CMakeFiles/test_stack.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/hassanamad/Projects/DataStructures/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_stack"
	cd /Users/hassanamad/Projects/DataStructures/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_stack.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test_stack.dir/build: tests/test_stack
.PHONY : tests/CMakeFiles/test_stack.dir/build

tests/CMakeFiles/test_stack.dir/clean:
	cd /Users/hassanamad/Projects/DataStructures/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_stack.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test_stack.dir/clean

tests/CMakeFiles/test_stack.dir/depend:
	cd /Users/hassanamad/Projects/DataStructures/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/hassanamad/Projects/DataStructures /Users/hassanamad/Projects/DataStructures/tests /Users/hassanamad/Projects/DataStructures/build /Users/hassanamad/Projects/DataStructures/build/tests /Users/hassanamad/Projects/DataStructures/build/tests/CMakeFiles/test_stack.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tests/CMakeFiles/test_stack.dir/depend

