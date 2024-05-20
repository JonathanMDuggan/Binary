cmake_minimum_required(VERSION 3.20)
message("Hello, this is the tests folder!")
file(GLOB_RECURSE TEST_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp")