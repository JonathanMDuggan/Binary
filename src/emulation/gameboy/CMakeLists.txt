cmake_minimum_required(VERSION 3.20)
include(../../../dep/CMakeLists.txt)
message("Hello, this is the gameboy emulator folder!")
file(GLOB_RECURSE RETRO_GB_SOURCE
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE RETRO_GB_HEADER
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp")