
cmake_minimum_required(VERSION 3.19)
project(Gameboy_Engine)
set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
set(DRIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/drivers")
set(DRIVER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/include")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

file(GLOB_RECURSE DRIVER_FILES ${DRIVER_DIR}/*.cpp)
file(GLOB_RECURSE DRIVER_INCLUDE_FILES ${DRIVER_INCLUDE_DIR}/*.h)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB_RECURSE VERT_SHADERS ${SHADERS_DIR}/*.vert)
file(GLOB_RECURSE FRAG_SHADERS ${SHADERS_DIR}/*.frag)

# Build and create shaders for the funny program
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
foreach(VERT_FILE ${VERT_SHADERS})
  execute_process(
    COMMAND ${GLSL_COMPILER} ${VERT_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(VERT_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile vertex shader!")
endif()

foreach(FRAG_FILE ${FRAG_SHADERS})
  execute_process(
    COMMAND ${GLSL_COMPILER} ${FRAG_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(FRAG_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile vertex shader!") 
endif()

add_executable(Gameboy_Engine 
  ${SRC_FILES} ${HEADER_FILES} ${DRIVER_DIR} ${DRIVER_INCLUDE_FILES} 
)

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
"\t\tThe Vulkan SDK:         $ENV{VULKAN_SDK}\n"
"\t\tCMAKE_PREFIX_PATH:      ${CMAKE_PREFIX_PATH}\n"
"\n[Gameboy_Engine]\n"
)