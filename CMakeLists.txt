
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)

add_executable(Gameboy_Engine 
  ${SRC_FILES}
  ${HEADER_FILES}
)
# SDL2 Libs 
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

#Vulkan API
find_package(Vulkan REQUIRED)

target_include_directories(Gameboy_Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Gameboy_Engine SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(Gameboy_Engine SDL2_image::SDL2_image)
target_link_libraries(Gameboy_Engine SDL2_mixer::SDL2_mixer)
target_link_libraries(Gameboy_Engine Vulkan::Vulkan)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Gameboy_Engine PROPERTY CXX_STANDARD 20)
endif()

file(COPY resources DESTINATION ${DST_DIR}/)