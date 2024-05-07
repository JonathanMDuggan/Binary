get_directory_property(hasParent PARENT_DIRECTORY)
message("Uh, This is the dep folder")
include_directories(imgui imgui/backends)
file(GLOB IMGUI_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl2.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl2.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
)
# TODO: Create a git submodule update cmake thing below
set(UPDATE_SUBMODULE "git submodule update --init --recursive")
execute_process(
  COMMAND  ${UPDATE_SUBMODULE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  RESULT_VARIABLE CMD_RESULT
)

if(CMD_RESULT EQUAL "0")
  message(FATAL_ERROR "failed to update submdoules: " ${CMD_RESULT})
endif()

