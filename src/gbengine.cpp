// File: gbengine.cpp
// Programmer: Jonathan M Duggan
// Date: 2024-03-14
// Purpose: Gameboy Engine start up file
// Version: 0.0.0
// 
// NOTE: This file is heavliy commented becasue I don't know what im writing
//       I'm following a guide online called https://vkguide.dev/.

#pragma once 
#define VMA_VULKAN_VERSION 1002000
#include "include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

void gbengine::GameBoyEngine::InitSDL() {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(app_info.name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, config.resolution.width,
                                config.resolution.height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}

void gbengine::GameBoyEngine::InitVulkan() { 
  InitVulkanInstance(); 
}

void gbengine::GameBoyEngine::InitVulkanApplication() {

}

void gbengine::GameBoyEngine::InitVulkanInstance() {
  
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                        nullptr)) {
    fmt::print(
        "Failed to get the number of Vulkan instance extensions from SDL\n");
    return;
  }

  std::vector<const char*> sdl_extensions(sdl_extension_count);
  if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                        sdl_extensions.data())) {
    fmt::print("Failed to get the Vulkan instance extensions from SDL\n");
    return;
  }

  vk.app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = app_info.name,
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = app_info.name,
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0,
  };

  std::vector<const char*> required_extensions(sdl_extensions.begin(),
                                               sdl_extensions.end());
  required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  vk.instance_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    .pApplicationInfo = &vk.app_info,
    .enabledLayerCount = 0,
    .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
    .ppEnabledExtensionNames = required_extensions.data(),
  };

  vk.result = vkCreateInstance(&vk.instance_info, nullptr, &vk.instance);
  if (vk.result != VK_SUCCESS) {
    fmt::print("Failed to create Vulkan instance\n");
    return;
  }

  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensions.data());

  fmt::print("Available extensions:\n");
  for (const auto& extension : extensions) {
    fmt::print("  {}\n", extension.extensionName);
  }

}

void gbengine::GameBoyEngine::InitVulkanValidationLayers() {
  uint32_t layer_count;
  bool layer_found = false;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* kLayerNames : kValidationLayers) {
    for (const auto& layer_properties: available_layers) {
      if (std::strcmp(kLayerNames, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }
    if (!layer_found) {
      
    }
  }
}

 void gbengine::GameBoyEngine::SDLPoolEvents(bool* running) {
  SDL_Event event;
   while (SDL_PollEvent(&event)) {
     switch (event.type) {
       case SDL_KEYDOWN:
         std::cout << "Key was pressed down\n";
         break;
       case SDL_QUIT:
         *running = false;
         return;
       default:
         break;
     }
   }
 }

 gbengine::GameBoyEngine::~GameBoyEngine(){
   // If i don't do this, memory corrupts.
   if (vk.instance) {
     vkDestroyInstance(vk.instance, nullptr);
     vk.instance = VK_NULL_HANDLE;
   }
   SDL_DestroyWindow(sdl.window);
   SDL_Quit();
 }

gbengine::GameBoyEngine::GameBoyEngine() {
   app_info.name            = GB_ENGINE_NAME;
   config.resolution.height = kStandardDefinitionHeight;
   config.resolution.width  = kStandardDefinitionWidth;
   InitSDL();
   InitVulkan();
   return;
}

