cmake_minimum_required(VERSION 3.9.2)
project(fbam C)

find_library(DL_LIB dl)
set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SHARED_LINKER_FLAGS "-ldl")

file (STRINGS "version.txt" VERSION_STRING)
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 MAJOR_VERSION)
list(GET VERSION_LIST 1 MINOR_VERSION)
list(GET VERSION_LIST 2 PATCH_VERSION)

add_library(blockaccess SHARED src/fbam.c src/fbam.h src/constants.h)
set_target_properties(blockaccess PROPERTIES VERSION ${VERSION_STRING} SOVERSION ${MAJOR_VERSION})
target_link_libraries(blockaccess "${DL_LIB}")
add_executable(testreader "testreader/reader.c")
add_executable(interceptor "testreader/interceptor.c")
