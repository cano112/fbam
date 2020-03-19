cmake_minimum_required(VERSION 3.13)
project(fbam C)

find_library(DL_LIB dl)
set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SHARED_LINKER_FLAGS "-ldl")

add_library(blockaccess SHARED block_access_decorator/block_access_decorator.c block_access_decorator/block_access_decorator.h common/commons.h)
target_link_libraries(blockaccess "${DL_LIB}")
add_executable(testreader "testreader/reader.c")


