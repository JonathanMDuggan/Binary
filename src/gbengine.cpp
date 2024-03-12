#pragma once 
#include "include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
namespace gbengine {
uint32_t GameBoyEngine::InitSDL() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(info.name, 0, 0, config.resolution.width,
                                config.resolution.height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  return EXIT_SUCCESS;
}
uint32_t GameBoyEngine::InitVulkan() {
  vulkan.info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = info.name,
    .applicationVersion = 0,
    .pEngineName = info.name,
    .engineVersion = 0,
    .apiVersion = VK_API_VERSION_1_3,
  };
  SDL_Vulkan_GetInstanceExtensions(sdl.window, sdl.pCount, sdl.pNames);
  vulkan.instance_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = NULL,
    .pApplicationInfo = &vulkan.info,
    .enabledLayerCount = 
    .};
}

GameBoyEngine::GameBoyEngine(uint32_t mode_flags) {
  if (InitSDL()) {
    std::cout << "Failed to create SDL window\n";
    return;
  }
  if (InitVulkan()) {
    std::cout << "Failed to initialize Vulkan\n";
  }
}
}
