
cmake_minimum_required(VERSION 3.8)

add_executable(Gameboy_Engine "src/main.cc" "src/include/gameboy_processor.h" "src/include/gameboy_opcode.h" "src/include/vulkan_config.h")

target_include_directories(Gameboy_Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Gameboy_Engine PROPERTY CXX_STANDARD 20)
endif()