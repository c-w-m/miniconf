cmake_minimum_required(VERSION 2.8)

project(miniconf)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -O0 -g -Wall -Werror")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

set(EX1_SRC examples/miniconf_example1.cpp)
set(EX2_SRC examples/miniconf_example2.cpp)

add_executable(miniconf_example1 ${EX1_SRC})
add_executable(miniconf_example2 ${EX2_SRC})
