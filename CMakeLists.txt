
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)

add_executable(Gameboy_Engine ${SRC_FILES} ${HEADER_FILES})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(dep/spdlog)
add_subdirectory(dep/VulkanMemoryAllocator)

target_include_directories(Gameboy_Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Gameboy_Engine PRIVATE 
  SDL2::SDL2 SDL2::SDL2main 
  SDL2_image::SDL2_image 
  SDL2_mixer::SDL2_mixer 
  Vulkan::Vulkan Vulkan::Headers 
  GPUOpen::VulkanMemoryAllocator 
  spdlog::spdlog_header_only
)
set_property(TARGET Gameboy_Engine PROPERTY CXX_STANDARD 20)
file(COPY resources DESTINATION ${DST_DIR}/)
message(
"\n[Gameboy_Engine]\n\n"
"\t\tThe Vulkan INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}\n"
"\t\tThe Vulkan LIBRARY:     ${Vulkan_LIBRARY}\n"
"\t\tCMAKE_PREFIX_PATH:      ${CMAKE_PREFIX_PATH}\n"
"\n[Gameboy_Engine]\n"
)