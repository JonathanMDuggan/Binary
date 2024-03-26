
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB_RECURSE VERT_SHADERS ${SHADERS_DIR}/*.vert)
file(GLOB_RECURSE FRAG_SHADERS ${SHADERS_DIR}/*.frag)
#set(VERT_SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv")
#set(FRAG_SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv")

# Build and create shaders for the funny program
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
foreach(VERT_FILE ${VERT_SHADERS})
  set(VERT_NAME ${VERT_FILE} NAME_WE)
  message( "${GLSL_COMPILER} ${VERT_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/")
  execute_process(
    COMMAND ${GLSL_COMPILER} ${VERT_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/${VERT_NAME}.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(VERT_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile vertex shader!")  # Display an error message and halt CMake if compilation failed
endif()

foreach(FRAG_FILE ${FRAG_SHADERS})
  set(FRAG_NAME ${FRAG_FILE} NAME_WE)
  execute_process(
    COMMAND ${GLSL_COMPILER} ${FRAG_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/${FRAG_NAME}.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(FRAG_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile vertex shader!")  # Display an error message and halt CMake if compilation failed
endif()

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
"\t\tThe Vulkan SDK:         $ENV{VULKAN_SDK}\n"
"\t\tCMAKE_PREFIX_PATH:      ${CMAKE_PREFIX_PATH}\n"
"\n[Gameboy_Engine]\n"
)