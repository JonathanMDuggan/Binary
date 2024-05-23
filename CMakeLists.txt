cmake_minimum_required(VERSION 3.20)
project(Binary)
set(CMAKE_CXX_STANDARD 20)
# Get the imgui stuff
# ImGui stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


#add_subdirectory(dep/fmt)
add_subdirectory(dep)
set(IMGUI_DIR "dep/imgui")
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
get_directory_property(IMGUI_SRC
                       DIRECTORY ${CMAKE_SOURCE_DIR}/dep
                       DEFINITION IMGUI_SRC)
add_subdirectory(dep/yaml-cpp)
add_subdirectory(dep/spdlog)
add_subdirectory(dep/VulkanMemoryAllocator)
add_subdirectory(dep/GLFW)
add_subdirectory("shaders/")
add_subdirectory("src/")
add_subdirectory(dep/nativefiledialog-extended)

get_directory_property(ALL_CPP_HEADER_FILES
                       DIRECTORY ${CMAKE_SOURCE_DIR}/src
                       DEFINITION ALL_CPP_HEADER_FILES)

get_directory_property(ALL_CPP_SRC_FILES
                       DIRECTORY ${CMAKE_SOURCE_DIR}/src
                       DEFINITION ALL_CPP_SRC_FILES)

# The programmer must install SDL2 binaries and Vulkan on their own computer
# and link the external cmake variables to the path of those libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

enable_testing()
file(GLOB BINARY_SOURCE_CODE
  ${IMGUI_SRC} 
  ${ALL_CPP_HEADER_FILES}
  ${ALL_CPP_SRC_FILES}
)
add_executable(Binary 
  ${BINARY_SOURCE_CODE}
)

# Google Test
add_subdirectory(dep/googletest)

target_include_directories(Binary PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(REQUIRED_LIBRARIES
  SDL2::SDL2
  SDL2::SDL2main
  SDL2_image::SDL2_image
  SDL2_mixer::SDL2_mixer
  Vulkan::Vulkan
  Vulkan::Headers
  GPUOpen::VulkanMemoryAllocator
  spdlog::spdlog
  spdlog::spdlog_header_only
  glfw
  OpenGL::GL
  GTest::gtest_main
  yaml-cpp
  nfd 
  GTest::gmock_main
  # fmt::fmt
)

target_link_libraries(Binary PRIVATE ${REQUIRED_LIBRARIES})

if(BUILD_TESTING)
  add_subdirectory(tests)
  get_directory_property(TEST_SOURCE_FILES
                       DIRECTORY ${CMAKE_SOURCE_DIR}/tests
                       DEFINITION TEST_SOURCE_FILES)

  add_executable(Binary_Test
    ${BINARY_SOURCE_CODE}
    ${TEST_SOURCE_FILES})

  target_include_directories(Binary_Test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(Binary_Test PRIVATE ${REQUIRED_LIBRARIES})
  target_compile_definitions(Binary_Test PRIVATE BINARY_TEST)

  # Include GoogleTest
  include(GoogleTest)
  gtest_discover_tests(Binary_Test)
endif()

find_path(SDL2_IMAGE_INCLUDE_DIR NAMES SDL2_image.h)
set_property(TARGET Binary PROPERTY CXX_STANDARD 20)

# Move the resource folder where the executable is 
file(COPY resources DESTINATION ${DST_DIR}/)
file(COPY config DESTINATION ${DST_DIR}/)
# Moving the SDL_Image.dll to the destination folder
file(COPY ${SDL2_image_DIR}/../lib/x64/SDL2_image.dll DESTINATION ${DST_DIR}/)

message("
================================== BINARY ==================================
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
================================ TEST FILES ================================")
foreach(TEST_FILE ${TEST_SOURCE_FILES})
  message("${TEST_FILE}")
endforeach(TEST_FILE)
message("
================================== IMGUI ===================================")
foreach(IMGUI_FILE ${IMGUI_SRC})
  message("${IMGUI_FILE}")
endforeach(IMGUI_FILE)