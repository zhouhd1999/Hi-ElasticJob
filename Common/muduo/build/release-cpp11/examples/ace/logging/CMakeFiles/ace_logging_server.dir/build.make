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
CMAKE_SOURCE_DIR = /root/muduo/muduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/muduo/build/release-cpp11

# Include any dependencies generated for this target.
include examples/ace/logging/CMakeFiles/ace_logging_server.dir/depend.make

# Include the progress variables for this target.
include examples/ace/logging/CMakeFiles/ace_logging_server.dir/progress.make

# Include the compile flags for this target's objects.
include examples/ace/logging/CMakeFiles/ace_logging_server.dir/flags.make

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o: examples/ace/logging/CMakeFiles/ace_logging_server.dir/flags.make
examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o: /root/muduo/muduo/examples/ace/logging/server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/muduo/build/release-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o"
	cd /root/muduo/build/release-cpp11/examples/ace/logging && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ace_logging_server.dir/server.cc.o -c /root/muduo/muduo/examples/ace/logging/server.cc

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ace_logging_server.dir/server.cc.i"
	cd /root/muduo/build/release-cpp11/examples/ace/logging && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/muduo/muduo/examples/ace/logging/server.cc > CMakeFiles/ace_logging_server.dir/server.cc.i

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ace_logging_server.dir/server.cc.s"
	cd /root/muduo/build/release-cpp11/examples/ace/logging && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/muduo/muduo/examples/ace/logging/server.cc -o CMakeFiles/ace_logging_server.dir/server.cc.s

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.requires:

.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.requires

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.provides: examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.requires
	$(MAKE) -f examples/ace/logging/CMakeFiles/ace_logging_server.dir/build.make examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.provides.build
.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.provides

examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.provides.build: examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o


# Object files for target ace_logging_server
ace_logging_server_OBJECTS = \
"CMakeFiles/ace_logging_server.dir/server.cc.o"

# External object files for target ace_logging_server
ace_logging_server_EXTERNAL_OBJECTS =

bin/ace_logging_server: examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o
bin/ace_logging_server: examples/ace/logging/CMakeFiles/ace_logging_server.dir/build.make
bin/ace_logging_server: lib/libmuduo_protobuf_codec.a
bin/ace_logging_server: lib/libace_logging_proto.a
bin/ace_logging_server: lib/libmuduo_net.a
bin/ace_logging_server: lib/libmuduo_base.a
bin/ace_logging_server: examples/ace/logging/CMakeFiles/ace_logging_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/muduo/build/release-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/ace_logging_server"
	cd /root/muduo/build/release-cpp11/examples/ace/logging && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ace_logging_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/ace/logging/CMakeFiles/ace_logging_server.dir/build: bin/ace_logging_server

.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/build

examples/ace/logging/CMakeFiles/ace_logging_server.dir/requires: examples/ace/logging/CMakeFiles/ace_logging_server.dir/server.cc.o.requires

.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/requires

examples/ace/logging/CMakeFiles/ace_logging_server.dir/clean:
	cd /root/muduo/build/release-cpp11/examples/ace/logging && $(CMAKE_COMMAND) -P CMakeFiles/ace_logging_server.dir/cmake_clean.cmake
.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/clean

examples/ace/logging/CMakeFiles/ace_logging_server.dir/depend:
	cd /root/muduo/build/release-cpp11 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/muduo/muduo /root/muduo/muduo/examples/ace/logging /root/muduo/build/release-cpp11 /root/muduo/build/release-cpp11/examples/ace/logging /root/muduo/build/release-cpp11/examples/ace/logging/CMakeFiles/ace_logging_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/ace/logging/CMakeFiles/ace_logging_server.dir/depend

