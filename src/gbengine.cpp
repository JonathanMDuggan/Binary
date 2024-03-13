#pragma once 
#include "include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
uint32_t gbengine::GameBoyEngine::InitSDL() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(app_info.name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, config.resolution.width,
                                config.resolution.height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  return EXIT_SUCCESS;
}
uint32_t gbengine::GameBoyEngine::InitVulkan() {

  std::vector<const char*> kExtensionNames;
  std::vector<const char*> kLayerNames{};

  SDL_Vulkan_GetInstanceExtensions(sdl.window, sdl.pCount, sdl.pNames);
  kExtensionNames.resize(*sdl.pCount);
  SDL_Vulkan_GetInstanceExtensions(sdl.window, sdl.pCount,
                                   kExtensionNames.data());

  vulkan.app_info = {
    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext              = NULL,
    .pApplicationName   = app_info.name,
    .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
    .pEngineName        = app_info.name,
    .engineVersion      = VK_MAKE_VERSION(0, 0, 0),
    .apiVersion         = VK_API_VERSION_1_3,
  };

  kLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");
  
  vulkan.sdl_extenstion_count = 0;
  vulkan.instance_info = {
    .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext                   = NULL,
    .pApplicationInfo        = &vulkan.app_info,
    .enabledLayerCount       = static_cast<uint32_t>(kLayerNames.size()),
    .ppEnabledLayerNames     = kLayerNames.data(),
    .enabledExtensionCount   = static_cast<uint32_t>(kExtensionNames.size()),
    .ppEnabledExtensionNames = kExtensionNames.data(),
  };

  vulkan.result = vkCreateInstance(&vulkan.instance_info, NULL, vulkan.inst);
  if (vulkan.result != VK_SUCCESS) {
    std::cout << "Failed to create instance\n"; 
  }
  SDL_Vulkan_CreateSurface(sdl.window, *vulkan.inst, &vulkan.surface);
  return EXIT_SUCCESS;
}

 gbengine::GameBoyEngine::GameBoyEngine(uint32_t mode_flags) {
  app_info.name = GB_ENGINE_NAME;
  if (InitSDL()) {
    std::cout << "Failed to create SDL window\n";
    return;
  }
  if (InitVulkan()) {
    std::cout << "Failed to initialize Vulkan\n";
  }
}

