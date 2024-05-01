cmake_minimum_required(VERSION 3.20)
project(Retro)
set(CMAKE_CXX_STANDARD 20)
# Get the imgui stuff
# ImGui stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# The programmer must install SDL2 and Vulkan on their own computer
#add_subdirectory(dep/fmt)
add_subdirectory(dep)
set(IMGUI_DIR "dep/imgui")
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
get_directory_property(IMGUI_SRC
                       DIRECTORY ${CMAKE_SOURCE_DIR}/dep
                       DEFINITION IMGUI_SRC)

add_subdirectory(dep/spdlog)
add_subdirectory(dep/VulkanMemoryAllocator)
add_subdirectory(dep/GLFW)
add_subdirectory("shaders/")
add_subdirectory("src/")

get_directory_property(ALL_CPP_HEADER_FILES
                       DIRECTORY ${CMAKE_SOURCE_DIR}/src
                       DEFINITION ALL_CPP_HEADER_FILES)

get_directory_property(ALL_CPP_SRC_FILES
                       DIRECTORY ${CMAKE_SOURCE_DIR}/src
                       DEFINITION ALL_CPP_SRC_FILES)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Gameboy_Engine 
${IMGUI_SRC} 
${ALL_CPP_HEADER_FILES} 
${ALL_CPP_SRC_FILES})

target_include_directories(Gameboy_Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Gameboy_Engine PRIVATE 
  SDL2::SDL2 SDL2::SDL2main
  SDL2_image::SDL2_image 
  SDL2_mixer::SDL2_mixer
  Vulkan::Vulkan Vulkan::Headers
  GPUOpen::VulkanMemoryAllocator
  spdlog::spdlog spdlog::spdlog_header_only
  glfw
  OpenGL::GL
  #fmt::fmt
)
find_path(SDL2_IMAGE_INCLUDE_DIR NAMES SDL2_image.h)
set_property(TARGET Gameboy_Engine PROPERTY CXX_STANDARD 20)

# Move the resource folder where the executable is 
file(COPY resources DESTINATION ${DST_DIR}/)

# Moving the SDL_Image.dll to the destination folder
file(COPY ${SDL2_image_DIR}/../lib/x64/SDL2_image.dll DESTINATION ${DST_DIR}/)

message("
============================== Gameboy_Engine ==============================
The Vulkan INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}
The Vulkan LIBRARY:     ${Vulkan_LIBRARY}
The Vulkan SDK:         $ENV{VULKAN_SDK}
CMAKE_PREFIX_PATH:      ${CMAKE_PREFIX_PATH}
SDL Image               ${SDL2_image_DIR}"
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
================================== IMGUI ===================================")
foreach(IMGUI_FILE ${IMGUI_SRC})
  message("${IMGUI_FILE}")
endforeach(IMGUI_FILE)