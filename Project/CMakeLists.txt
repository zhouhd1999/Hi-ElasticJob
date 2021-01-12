cmake_minimum_required(VERSION 2.6)

project(MY CXX)

if(NOT MUDUO_PATH)
  set(MUDUO_PATH "/root/cpp/Common/muduo/build/release-install-cpp11")
endif()

add_definitions(-std=c++11)

set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -Wall
 -Wextra
 # -m32
 # -Werror
 # -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_path(Muduo_INCLUDE_DIR muduo "${MUDUO_PATH}/include")
find_path(Muduo_LIBRARY_DIR libmuduo_net.a "${MUDUO_PATH}/lib")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${Muduo_LIBRARY_DIR})
message(STATUS ${Muduo_INCLUDE_DIR})
message(STATUS ${Muduo_LIBRARY_DIR})

include_directories(/root/cpp/Project/src/base) #添加base头文件
include_directories(${Muduo_INCLUDE_DIR})

ADD_SUBDIRECTORY(./src/base)  # 进入base目录


find_library(muduo_base muduo_base)
find_library(muduo_net muduo_net)
message(STATUS ${muduo_base})
message(STATUS ${muduo_net})

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR})


add_executable(hello ./src/hello.cc)
target_link_libraries(hello 
  ${muduo_net}
  ${muduo_base}
  Base
  pthread rt
)

# 测试用例
add_executable(DBCommonTest ./src/base/test/DBCommonTest.cc)
target_link_libraries(DBCommonTest
  ${muduo_net}
  ${muduo_base}
  Base
  pthread rt
  ${Boost_LIBRARIES}
)

add_executable(DBTransactionTest ./src/base/test/DBTransactionTest.cc)
target_link_libraries(DBTransactionTest
  ${muduo_net}
  ${muduo_base}
  Base
  pthread rt
  ${Boost_LIBRARIES}
)

add_executable(RedisTest ./src/base/test/RedisTest.cc)
target_link_libraries(RedisTest
  ${muduo_net}
  ${muduo_base}
  redis++
  hiredis
  Base
  pthread rt
  ${Boost_LIBRARIES}
)

add_executable(TimerTest ./src/base/test/TimerTest.cc)
target_link_libraries(TimerTest
  ${muduo_net}
  ${muduo_base}
  ${Boost_LIBRARIES}
  Base
  pthread rt
)
