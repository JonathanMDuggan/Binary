cmake_minimum_required(VERSION 3.20)
include(../dep/CMakeLists.txt)
message("Hello, this is the src folder!")
file(GLOB_RECURSE ALL_CPP_SRC_FILES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE ALL_CPP_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
