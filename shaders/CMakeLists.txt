message("Hola, this is the shaders folder!")
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
  set(GLSL_COMPILER  "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB VERT_SHADERS ${CMAKE_CURRENT_LIST_DIR}/*.vert)
file(GLOB FRAG_SHADERS ${CMAKE_CURRENT_LIST_DIR}/*.frag)

# Build and create shaders for the funny program
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
foreach(VERT_FILE ${VERT_SHADERS})
  execute_process(
    COMMAND ${GLSL_COMPILER} ${VERT_FILE} -o ${CMAKE_BINARY_DIR}/shaders/vert.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(VERT_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile vertex shader!: " ${CMD_RESULT})
endif()

foreach(FRAG_FILE ${FRAG_SHADERS})
  execute_process(
    COMMAND ${GLSL_COMPILER} ${FRAG_FILE} -o ${CMAKE_BINARY_DIR}/shaders/frag.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    RESULT_VARIABLE CMD_RESULT
  )
endforeach(FRAG_FILE)

if(CMD_RESULT)
    message(FATAL_ERROR "Failed to compile frag shader!: " ${CMD_RESULT}) 
endif()