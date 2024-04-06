cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Main source directory
set(MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main")
set(MAIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main/include")

# Driver source directory
set(DRIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/drivers")
set(DRIVER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/include")

# Input and output source directory
set(IO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/io")
set(IO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/io/include")

# Graphic user interface source directory
set(GUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/gui")
set(GUI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include/gui")

# Shader source directory
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# Driver source directory 
file(GLOB_RECURSE DRIVER_FILES ${DRIVER_DIR}/*.cpp)
file(GLOB_RECURSE DRIVER_HEADER_FILES ${DRIVER_INCLUDE_DIR}/*.h)

# IO source files 
file(GLOB_RECURSE IO_FILES ${IO_DIR}/*.cpp)
file(GLOB_RECURSE IO_INCLUDE_FILES ${IO_INCLUDE_DIR}/*.h)

# Main source files
file(GLOB_RECURSE MAIN_SRC_FILES ${MAIN_SRC_DIR}/*.cpp)
file(GLOB_RECURSE MAIN_HEADER_FILES ${MAIN_INCLUDE_DIR}/*.h)

# Put all header and source fils in one variable
set(ALL_CPP_SRC_FILES ${MAIN_SRC_FILES} ${DRIVER_FILES})
set(ALL_CPP_HEADER_FILES ${MAIN_HEADER_FILES} ${DRIVER_HEADER_FILES})

# CMake implementation for compiling GLSL shaders from the 
# shader directory
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
  ${ALL_CPP_SRC_FILES} 
  ${ALL_CPP_HEADER_FILES}
)

# The programmer must install SDL2 and Vulkan on their own computer
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
  spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
)
set_property(TARGET Gameboy_Engine PROPERTY CXX_STANDARD 20)

# Move the resource folder where the executable is 
file(COPY resources DESTINATION ${DST_DIR}/)
# Debuging messages
# Tells me where CMake found the vulkan lib
message("
============================== Gameboy_Engine ==============================
The Vulkan INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}
The Vulkan LIBRARY:     ${Vulkan_LIBRARY}
The Vulkan SDK:         $ENV{VULKAN_SDK}
CMAKE_PREFIX_PATH:      ${CMAKE_PREFIX_PATH}"
)

message("
============================= CPP SOURCE FILES =============================")
foreach(CPP_FILE ${ALL_CPP_SRC_FILES})
  message("${CPP_FILE}")
endforeach(CPP_FILE)
message("
=============================== HEADER FILES ===============================")
foreach(HEADER_FILE ${ALL_CPP_HEADER_FILES})
  message("${HEADER_FILE}")
endforeach(HEADER_FILE)
message("
============================================================================")