# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /root/cpp/Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/cpp/Project/build

# Include any dependencies generated for this target.
include CMakeFiles/DBTransactionTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/DBTransactionTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/DBTransactionTest.dir/flags.make

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o: CMakeFiles/DBTransactionTest.dir/flags.make
CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o: ../src/base/test/DBTransactionTest.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o -c /root/cpp/Project/src/base/test/DBTransactionTest.cc

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/test/DBTransactionTest.cc > CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.i

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/test/DBTransactionTest.cc -o CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.s

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.requires:

.PHONY : CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.requires

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.provides: CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.requires
	$(MAKE) -f CMakeFiles/DBTransactionTest.dir/build.make CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.provides.build
.PHONY : CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.provides

CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.provides.build: CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o


# Object files for target DBTransactionTest
DBTransactionTest_OBJECTS = \
"CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o"

# External object files for target DBTransactionTest
DBTransactionTest_EXTERNAL_OBJECTS =

bin/DBTransactionTest: CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o
bin/DBTransactionTest: CMakeFiles/DBTransactionTest.dir/build.make
bin/DBTransactionTest: /root/cpp/Common/muduo/build/release-install-cpp11/lib/libmuduo_net.a
bin/DBTransactionTest: /root/cpp/Common/muduo/build/release-install-cpp11/lib/libmuduo_base.a
bin/DBTransactionTest: lib/libBase.a
bin/DBTransactionTest: /root/cpp/Common/muduo/build/release-install-cpp11/lib/libmuduo_net.a
bin/DBTransactionTest: /root/cpp/Common/muduo/build/release-install-cpp11/lib/libmuduo_base.a
bin/DBTransactionTest: CMakeFiles/DBTransactionTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/DBTransactionTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/DBTransactionTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/DBTransactionTest.dir/build: bin/DBTransactionTest

.PHONY : CMakeFiles/DBTransactionTest.dir/build

CMakeFiles/DBTransactionTest.dir/requires: CMakeFiles/DBTransactionTest.dir/src/base/test/DBTransactionTest.cc.o.requires

.PHONY : CMakeFiles/DBTransactionTest.dir/requires

CMakeFiles/DBTransactionTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/DBTransactionTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/DBTransactionTest.dir/clean

CMakeFiles/DBTransactionTest.dir/depend:
	cd /root/cpp/Project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/cpp/Project /root/cpp/Project /root/cpp/Project/build /root/cpp/Project/build /root/cpp/Project/build/CMakeFiles/DBTransactionTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/DBTransactionTest.dir/depend
