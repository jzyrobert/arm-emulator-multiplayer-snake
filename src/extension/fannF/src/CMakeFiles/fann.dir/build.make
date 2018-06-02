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
include src/CMakeFiles/fann.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/fann.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/fann.dir/flags.make

src/CMakeFiles/fann.dir/floatfann.c.o: src/CMakeFiles/fann.dir/flags.make
src/CMakeFiles/fann.dir/floatfann.c.o: src/floatfann.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/fann.dir/floatfann.c.o"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fann.dir/floatfann.c.o   -c /home/rob/Documents/fann/src/floatfann.c

src/CMakeFiles/fann.dir/floatfann.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fann.dir/floatfann.c.i"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rob/Documents/fann/src/floatfann.c > CMakeFiles/fann.dir/floatfann.c.i

src/CMakeFiles/fann.dir/floatfann.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fann.dir/floatfann.c.s"
	cd /home/rob/Documents/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rob/Documents/fann/src/floatfann.c -o CMakeFiles/fann.dir/floatfann.c.s

src/CMakeFiles/fann.dir/floatfann.c.o.requires:

.PHONY : src/CMakeFiles/fann.dir/floatfann.c.o.requires

src/CMakeFiles/fann.dir/floatfann.c.o.provides: src/CMakeFiles/fann.dir/floatfann.c.o.requires
	$(MAKE) -f src/CMakeFiles/fann.dir/build.make src/CMakeFiles/fann.dir/floatfann.c.o.provides.build
.PHONY : src/CMakeFiles/fann.dir/floatfann.c.o.provides

src/CMakeFiles/fann.dir/floatfann.c.o.provides.build: src/CMakeFiles/fann.dir/floatfann.c.o


# Object files for target fann
fann_OBJECTS = \
"CMakeFiles/fann.dir/floatfann.c.o"

# External object files for target fann
fann_EXTERNAL_OBJECTS =

src/libfann.so.2.2.0: src/CMakeFiles/fann.dir/floatfann.c.o
src/libfann.so.2.2.0: src/CMakeFiles/fann.dir/build.make
src/libfann.so.2.2.0: src/CMakeFiles/fann.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libfann.so"
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fann.dir/link.txt --verbose=$(VERBOSE)
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -E cmake_symlink_library libfann.so.2.2.0 libfann.so.2 libfann.so

src/libfann.so.2: src/libfann.so.2.2.0
	@$(CMAKE_COMMAND) -E touch_nocreate src/libfann.so.2

src/libfann.so: src/libfann.so.2.2.0
	@$(CMAKE_COMMAND) -E touch_nocreate src/libfann.so

# Rule to build all files generated by this target.
src/CMakeFiles/fann.dir/build: src/libfann.so

.PHONY : src/CMakeFiles/fann.dir/build

src/CMakeFiles/fann.dir/requires: src/CMakeFiles/fann.dir/floatfann.c.o.requires

.PHONY : src/CMakeFiles/fann.dir/requires

src/CMakeFiles/fann.dir/clean:
	cd /home/rob/Documents/fann/src && $(CMAKE_COMMAND) -P CMakeFiles/fann.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/fann.dir/clean

src/CMakeFiles/fann.dir/depend:
	cd /home/rob/Documents/fann && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rob/Documents/fann /home/rob/Documents/fann/src /home/rob/Documents/fann /home/rob/Documents/fann/src /home/rob/Documents/fann/src/CMakeFiles/fann.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/fann.dir/depend

