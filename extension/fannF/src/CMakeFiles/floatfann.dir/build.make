# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rob/Documents/fann

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rob/Documents/fann

# Include any dependencies generated for this target.
include src/CMakeFiles/floatfann.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/floatfann.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/floatfann.dir/flags.make

src/CMakeFiles/floatfann.dir/floatfann.c.o: src/CMakeFiles/floatfann.dir/flags.make
src/CMakeFiles/floatfann.dir/floatfann.c.o: src/floatfann.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/floatfann.dir/floatfann.c.o"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/floatfann.dir/floatfann.c.o   -c /home/rob/Documents/fann/src/floatfann.c

src/CMakeFiles/floatfann.dir/floatfann.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/floatfann.dir/floatfann.c.i"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rob/Documents/fann/src/floatfann.c > CMakeFiles/floatfann.dir/floatfann.c.i

src/CMakeFiles/floatfann.dir/floatfann.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/floatfann.dir/floatfann.c.s"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rob/Documents/fann/src/floatfann.c -o CMakeFiles/floatfann.dir/floatfann.c.s

src/CMakeFiles/floatfann.dir/floatfann.c.o.requires:

.PHONY : src/CMakeFiles/floatfann.dir/floatfann.c.o.requires

src/CMakeFiles/floatfann.dir/floatfann.c.o.provides: src/CMakeFiles/floatfann.dir/floatfann.c.o.requires
	$(MAKE) -f src/CMakeFiles/floatfann.dir/build.make src/CMakeFiles/floatfann.dir/floatfann.c.o.provides.build
.PHONY : src/CMakeFiles/floatfann.dir/floatfann.c.o.provides

src/CMakeFiles/floatfann.dir/floatfann.c.o.provides.build: src/CMakeFiles/floatfann.dir/floatfann.c.o


# Object files for target floatfann
floatfann_OBJECTS = \
"CMakeFiles/floatfann.dir/floatfann.c.o"

# External object files for target floatfann
floatfann_EXTERNAL_OBJECTS =

src/libfloatfann.so.2.2.0: src/CMakeFiles/floatfann.dir/floatfann.c.o
src/libfloatfann.so.2.2.0: src/CMakeFiles/floatfann.dir/build.make
src/libfloatfann.so.2.2.0: src/CMakeFiles/floatfann.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libfloatfann.so"
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/floatfann.dir/link.txt --verbose=$(VERBOSE)
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -E cmake_symlink_library libfloatfann.so.2.2.0 libfloatfann.so.2 libfloatfann.so

src/libfloatfann.so.2: src/libfloatfann.so.2.2.0
	@$(CMAKE_COMMAND) -E touch_nocreate src/libfloatfann.so.2

src/libfloatfann.so: src/libfloatfann.so.2.2.0
	@$(CMAKE_COMMAND) -E touch_nocreate src/libfloatfann.so

# Rule to build all files generated by this target.
src/CMakeFiles/floatfann.dir/build: src/libfloatfann.so

.PHONY : src/CMakeFiles/floatfann.dir/build

src/CMakeFiles/floatfann.dir/requires: src/CMakeFiles/floatfann.dir/floatfann.c.o.requires

.PHONY : src/CMakeFiles/floatfann.dir/requires

src/CMakeFiles/floatfann.dir/clean:
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -P CMakeFiles/floatfann.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/floatfann.dir/clean

src/CMakeFiles/floatfann.dir/depend:
	cd /home/rob/Documents/fann && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rob/Documents/fann /home/rob/Documents/fann/src /home/rob/Documents/fann /home/rob/Documents/fann/src /home/rob/Documents/fann/src/CMakeFiles/floatfann.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/floatfann.dir/depend

