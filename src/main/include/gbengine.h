#pragma once
#include <string>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <limits>
#include <fstream>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "vk_mem_alloc.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "spdlog/spdlog.h"
namespace retro{
typedef enum RendererType  { 
  k_None = 0,
  k_Vulkan = 1,
  k_OpenGL = 2
} RendererType;
typedef struct Application {
  std::string name;
  uint32_t version;
  uint32_t width;
  uint32_t height;
  RendererType renderer;
} Application; 
}  // namespace retro
