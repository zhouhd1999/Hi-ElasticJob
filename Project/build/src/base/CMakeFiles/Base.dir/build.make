# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
include src/base/CMakeFiles/Base.dir/depend.make

# Include the progress variables for this target.
include src/base/CMakeFiles/Base.dir/progress.make

# Include the compile flags for this target's objects.
include src/base/CMakeFiles/Base.dir/flags.make

src/base/CMakeFiles/Base.dir/ConfigManager.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/ConfigManager.cc.o: ../src/base/ConfigManager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/base/CMakeFiles/Base.dir/ConfigManager.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/ConfigManager.cc.o -c /root/cpp/Project/src/base/ConfigManager.cc

src/base/CMakeFiles/Base.dir/ConfigManager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/ConfigManager.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/ConfigManager.cc > CMakeFiles/Base.dir/ConfigManager.cc.i

src/base/CMakeFiles/Base.dir/ConfigManager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/ConfigManager.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/ConfigManager.cc -o CMakeFiles/Base.dir/ConfigManager.cc.s

src/base/CMakeFiles/Base.dir/DBBaseConn.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/DBBaseConn.cc.o: ../src/base/DBBaseConn.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/base/CMakeFiles/Base.dir/DBBaseConn.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/DBBaseConn.cc.o -c /root/cpp/Project/src/base/DBBaseConn.cc

src/base/CMakeFiles/Base.dir/DBBaseConn.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/DBBaseConn.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/DBBaseConn.cc > CMakeFiles/Base.dir/DBBaseConn.cc.i

src/base/CMakeFiles/Base.dir/DBBaseConn.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/DBBaseConn.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/DBBaseConn.cc -o CMakeFiles/Base.dir/DBBaseConn.cc.s

src/base/CMakeFiles/Base.dir/DBConnPool.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/DBConnPool.cc.o: ../src/base/DBConnPool.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/base/CMakeFiles/Base.dir/DBConnPool.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/DBConnPool.cc.o -c /root/cpp/Project/src/base/DBConnPool.cc

src/base/CMakeFiles/Base.dir/DBConnPool.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/DBConnPool.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/DBConnPool.cc > CMakeFiles/Base.dir/DBConnPool.cc.i

src/base/CMakeFiles/Base.dir/DBConnPool.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/DBConnPool.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/DBConnPool.cc -o CMakeFiles/Base.dir/DBConnPool.cc.s

src/base/CMakeFiles/Base.dir/DBManager.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/DBManager.cc.o: ../src/base/DBManager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/base/CMakeFiles/Base.dir/DBManager.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/DBManager.cc.o -c /root/cpp/Project/src/base/DBManager.cc

src/base/CMakeFiles/Base.dir/DBManager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/DBManager.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/DBManager.cc > CMakeFiles/Base.dir/DBManager.cc.i

src/base/CMakeFiles/Base.dir/DBManager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/DBManager.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/DBManager.cc -o CMakeFiles/Base.dir/DBManager.cc.s

src/base/CMakeFiles/Base.dir/Error.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/Error.cc.o: ../src/base/Error.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/base/CMakeFiles/Base.dir/Error.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/Error.cc.o -c /root/cpp/Project/src/base/Error.cc

src/base/CMakeFiles/Base.dir/Error.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/Error.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/Error.cc > CMakeFiles/Base.dir/Error.cc.i

src/base/CMakeFiles/Base.dir/Error.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/Error.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/Error.cc -o CMakeFiles/Base.dir/Error.cc.s

src/base/CMakeFiles/Base.dir/GuidUtil.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/GuidUtil.cc.o: ../src/base/GuidUtil.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/base/CMakeFiles/Base.dir/GuidUtil.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/GuidUtil.cc.o -c /root/cpp/Project/src/base/GuidUtil.cc

src/base/CMakeFiles/Base.dir/GuidUtil.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/GuidUtil.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/GuidUtil.cc > CMakeFiles/Base.dir/GuidUtil.cc.i

src/base/CMakeFiles/Base.dir/GuidUtil.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/GuidUtil.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/GuidUtil.cc -o CMakeFiles/Base.dir/GuidUtil.cc.s

src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o: ../src/base/MessageLoopByThreadPool.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o -c /root/cpp/Project/src/base/MessageLoopByThreadPool.cc

src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/MessageLoopByThreadPool.cc > CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.i

src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/MessageLoopByThreadPool.cc -o CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.s

src/base/CMakeFiles/Base.dir/RedisController.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/RedisController.cc.o: ../src/base/RedisController.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/base/CMakeFiles/Base.dir/RedisController.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/RedisController.cc.o -c /root/cpp/Project/src/base/RedisController.cc

src/base/CMakeFiles/Base.dir/RedisController.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/RedisController.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/RedisController.cc > CMakeFiles/Base.dir/RedisController.cc.i

src/base/CMakeFiles/Base.dir/RedisController.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/RedisController.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/RedisController.cc -o CMakeFiles/Base.dir/RedisController.cc.s

src/base/CMakeFiles/Base.dir/ThreadUtils.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/ThreadUtils.cc.o: ../src/base/ThreadUtils.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object src/base/CMakeFiles/Base.dir/ThreadUtils.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/ThreadUtils.cc.o -c /root/cpp/Project/src/base/ThreadUtils.cc

src/base/CMakeFiles/Base.dir/ThreadUtils.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/ThreadUtils.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/ThreadUtils.cc > CMakeFiles/Base.dir/ThreadUtils.cc.i

src/base/CMakeFiles/Base.dir/ThreadUtils.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/ThreadUtils.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/ThreadUtils.cc -o CMakeFiles/Base.dir/ThreadUtils.cc.s

src/base/CMakeFiles/Base.dir/Time.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/Time.cc.o: ../src/base/Time.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object src/base/CMakeFiles/Base.dir/Time.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/Time.cc.o -c /root/cpp/Project/src/base/Time.cc

src/base/CMakeFiles/Base.dir/Time.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/Time.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/Time.cc > CMakeFiles/Base.dir/Time.cc.i

src/base/CMakeFiles/Base.dir/Time.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/Time.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/Time.cc -o CMakeFiles/Base.dir/Time.cc.s

src/base/CMakeFiles/Base.dir/Timer.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/Timer.cc.o: ../src/base/Timer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object src/base/CMakeFiles/Base.dir/Timer.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/Timer.cc.o -c /root/cpp/Project/src/base/Timer.cc

src/base/CMakeFiles/Base.dir/Timer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/Timer.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/Timer.cc > CMakeFiles/Base.dir/Timer.cc.i

src/base/CMakeFiles/Base.dir/Timer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/Timer.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/Timer.cc -o CMakeFiles/Base.dir/Timer.cc.s

src/base/CMakeFiles/Base.dir/TimerCounter.cc.o: src/base/CMakeFiles/Base.dir/flags.make
src/base/CMakeFiles/Base.dir/TimerCounter.cc.o: ../src/base/TimerCounter.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object src/base/CMakeFiles/Base.dir/TimerCounter.cc.o"
	cd /root/cpp/Project/build/src/base && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Base.dir/TimerCounter.cc.o -c /root/cpp/Project/src/base/TimerCounter.cc

src/base/CMakeFiles/Base.dir/TimerCounter.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Base.dir/TimerCounter.cc.i"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/cpp/Project/src/base/TimerCounter.cc > CMakeFiles/Base.dir/TimerCounter.cc.i

src/base/CMakeFiles/Base.dir/TimerCounter.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Base.dir/TimerCounter.cc.s"
	cd /root/cpp/Project/build/src/base && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/cpp/Project/src/base/TimerCounter.cc -o CMakeFiles/Base.dir/TimerCounter.cc.s

# Object files for target Base
Base_OBJECTS = \
"CMakeFiles/Base.dir/ConfigManager.cc.o" \
"CMakeFiles/Base.dir/DBBaseConn.cc.o" \
"CMakeFiles/Base.dir/DBConnPool.cc.o" \
"CMakeFiles/Base.dir/DBManager.cc.o" \
"CMakeFiles/Base.dir/Error.cc.o" \
"CMakeFiles/Base.dir/GuidUtil.cc.o" \
"CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o" \
"CMakeFiles/Base.dir/RedisController.cc.o" \
"CMakeFiles/Base.dir/ThreadUtils.cc.o" \
"CMakeFiles/Base.dir/Time.cc.o" \
"CMakeFiles/Base.dir/Timer.cc.o" \
"CMakeFiles/Base.dir/TimerCounter.cc.o"

# External object files for target Base
Base_EXTERNAL_OBJECTS =

lib/libBase.a: src/base/CMakeFiles/Base.dir/ConfigManager.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/DBBaseConn.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/DBConnPool.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/DBManager.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/Error.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/GuidUtil.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/MessageLoopByThreadPool.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/RedisController.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/ThreadUtils.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/Time.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/Timer.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/TimerCounter.cc.o
lib/libBase.a: src/base/CMakeFiles/Base.dir/build.make
lib/libBase.a: src/base/CMakeFiles/Base.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/cpp/Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking CXX static library ../../lib/libBase.a"
	cd /root/cpp/Project/build/src/base && $(CMAKE_COMMAND) -P CMakeFiles/Base.dir/cmake_clean_target.cmake
	cd /root/cpp/Project/build/src/base && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Base.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/base/CMakeFiles/Base.dir/build: lib/libBase.a

.PHONY : src/base/CMakeFiles/Base.dir/build

src/base/CMakeFiles/Base.dir/clean:
	cd /root/cpp/Project/build/src/base && $(CMAKE_COMMAND) -P CMakeFiles/Base.dir/cmake_clean.cmake
.PHONY : src/base/CMakeFiles/Base.dir/clean

src/base/CMakeFiles/Base.dir/depend:
	cd /root/cpp/Project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/cpp/Project /root/cpp/Project/src/base /root/cpp/Project/build /root/cpp/Project/build/src/base /root/cpp/Project/build/src/base/CMakeFiles/Base.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/base/CMakeFiles/Base.dir/depend

