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
include tests/CMakeFiles/fann_tests.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/fann_tests.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/fann_tests.dir/flags.make

tests/CMakeFiles/fann_tests.dir/main.cpp.o: tests/CMakeFiles/fann_tests.dir/flags.make
tests/CMakeFiles/fann_tests.dir/main.cpp.o: tests/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/fann_tests.dir/main.cpp.o"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fann_tests.dir/main.cpp.o -c /home/rob/Documents/fann/tests/main.cpp

tests/CMakeFiles/fann_tests.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fann_tests.dir/main.cpp.i"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rob/Documents/fann/tests/main.cpp > CMakeFiles/fann_tests.dir/main.cpp.i

tests/CMakeFiles/fann_tests.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fann_tests.dir/main.cpp.s"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rob/Documents/fann/tests/main.cpp -o CMakeFiles/fann_tests.dir/main.cpp.s

tests/CMakeFiles/fann_tests.dir/main.cpp.o.requires:

.PHONY : tests/CMakeFiles/fann_tests.dir/main.cpp.o.requires

tests/CMakeFiles/fann_tests.dir/main.cpp.o.provides: tests/CMakeFiles/fann_tests.dir/main.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/fann_tests.dir/build.make tests/CMakeFiles/fann_tests.dir/main.cpp.o.provides.build
.PHONY : tests/CMakeFiles/fann_tests.dir/main.cpp.o.provides

tests/CMakeFiles/fann_tests.dir/main.cpp.o.provides.build: tests/CMakeFiles/fann_tests.dir/main.cpp.o


tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o: tests/CMakeFiles/fann_tests.dir/flags.make
tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o: tests/fann_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fann_tests.dir/fann_test.cpp.o -c /home/rob/Documents/fann/tests/fann_test.cpp

tests/CMakeFiles/fann_tests.dir/fann_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fann_tests.dir/fann_test.cpp.i"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rob/Documents/fann/tests/fann_test.cpp > CMakeFiles/fann_tests.dir/fann_test.cpp.i

tests/CMakeFiles/fann_tests.dir/fann_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fann_tests.dir/fann_test.cpp.s"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rob/Documents/fann/tests/fann_test.cpp -o CMakeFiles/fann_tests.dir/fann_test.cpp.s

tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.requires:

.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.requires

tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.provides: tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/fann_tests.dir/build.make tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.provides.build
.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.provides

tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.provides.build: tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o


tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o: tests/CMakeFiles/fann_tests.dir/flags.make
tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o: tests/fann_test_data.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fann_tests.dir/fann_test_data.cpp.o -c /home/rob/Documents/fann/tests/fann_test_data.cpp

tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fann_tests.dir/fann_test_data.cpp.i"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rob/Documents/fann/tests/fann_test_data.cpp > CMakeFiles/fann_tests.dir/fann_test_data.cpp.i

tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fann_tests.dir/fann_test_data.cpp.s"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rob/Documents/fann/tests/fann_test_data.cpp -o CMakeFiles/fann_tests.dir/fann_test_data.cpp.s

tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.requires:

.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.requires

tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.provides: tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/fann_tests.dir/build.make tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.provides.build
.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.provides

tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.provides.build: tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o


tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o: tests/CMakeFiles/fann_tests.dir/flags.make
tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o: tests/fann_test_train.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fann_tests.dir/fann_test_train.cpp.o -c /home/rob/Documents/fann/tests/fann_test_train.cpp

tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fann_tests.dir/fann_test_train.cpp.i"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rob/Documents/fann/tests/fann_test_train.cpp > CMakeFiles/fann_tests.dir/fann_test_train.cpp.i

tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fann_tests.dir/fann_test_train.cpp.s"
	cd /home/rob/Documents/fann/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rob/Documents/fann/tests/fann_test_train.cpp -o CMakeFiles/fann_tests.dir/fann_test_train.cpp.s

tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.requires:

.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.requires

tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.provides: tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/fann_tests.dir/build.make tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.provides.build
.PHONY : tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.provides

tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.provides.build: tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o


# Object files for target fann_tests
fann_tests_OBJECTS = \
"CMakeFiles/fann_tests.dir/main.cpp.o" \
"CMakeFiles/fann_tests.dir/fann_test.cpp.o" \
"CMakeFiles/fann_tests.dir/fann_test_data.cpp.o" \
"CMakeFiles/fann_tests.dir/fann_test_train.cpp.o"

# External object files for target fann_tests
fann_tests_EXTERNAL_OBJECTS =

tests/fann_tests: tests/CMakeFiles/fann_tests.dir/main.cpp.o
tests/fann_tests: tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o
tests/fann_tests: tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o
tests/fann_tests: tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o
tests/fann_tests: tests/CMakeFiles/fann_tests.dir/build.make
tests/fann_tests: lib/googletest/libgtest.so
tests/fann_tests: src/libdoublefann.so.2.2.0
tests/fann_tests: tests/CMakeFiles/fann_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rob/Documents/fann/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable fann_tests"
	cd /home/rob/Documents/fann/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fann_tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/fann_tests.dir/build: tests/fann_tests

.PHONY : tests/CMakeFiles/fann_tests.dir/build

tests/CMakeFiles/fann_tests.dir/requires: tests/CMakeFiles/fann_tests.dir/main.cpp.o.requires
tests/CMakeFiles/fann_tests.dir/requires: tests/CMakeFiles/fann_tests.dir/fann_test.cpp.o.requires
tests/CMakeFiles/fann_tests.dir/requires: tests/CMakeFiles/fann_tests.dir/fann_test_data.cpp.o.requires
tests/CMakeFiles/fann_tests.dir/requires: tests/CMakeFiles/fann_tests.dir/fann_test_train.cpp.o.requires

.PHONY : tests/CMakeFiles/fann_tests.dir/requires

tests/CMakeFiles/fann_tests.dir/clean:
	cd /home/rob/Documents/fann/tests && $(CMAKE_COMMAND) -P CMakeFiles/fann_tests.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/fann_tests.dir/clean

tests/CMakeFiles/fann_tests.dir/depend:
	cd /home/rob/Documents/fann && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rob/Documents/fann /home/rob/Documents/fann/tests /home/rob/Documents/fann /home/rob/Documents/fann/tests /home/rob/Documents/fann/tests/CMakeFiles/fann_tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/fann_tests.dir/depend

