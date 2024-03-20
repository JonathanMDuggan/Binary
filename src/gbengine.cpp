// File: gbengine.cpp
// Programmer: Jonathan M Duggan
// Date: 2024-03-14
// Purpose: Gameboy Engine start up file
// Version: 0.0.
// 
// NOTE: This file is heavliy commented becasue I don't know what im writing
//       I'm following a guide online called https://vkguide.dev/. Vulkan is
//       Complex, I need explanation. 

#pragma once 
#define VMA_VULKAN_VERSION 1002000
#include "include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void gbengine::GameBoyEngine::SetupDebugMessenger() { 
  if (!ValidationLayersEnabled) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  FillDebugMessengerCreateInfo(debug_info);

  if (CreateDebugUtilsMessengerEXT(vk.instance, &debug_info ,nullptr, &vk.debug_messenger) != VK_SUCCESS){
    fmt::print("failed to set up debug messenger!");
  }
}
VkResult gbengine::GameBoyEngine::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, 
        "vkDestoryDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}
void gbengine::GameBoyEngine::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestoryDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }

}


void gbengine::GameBoyEngine::FillDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& debug_info) {
  debug_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,

      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,

      .pfnUserCallback = debugCallback,
      .pUserData = nullptr,
  };
}



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
  VkInstanceCreateInfo instance_info{};
  if (ValidationLayersEnabled && !VulkanValidationLayerSupported()) {
    fmt::print("Validation layers requested, but not available!");
  }

  vk.app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = app_info.name,
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = app_info.name,
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0,
  };
  //required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &vk.app_info;

  std::vector<const char*> sdl_extensions = GetExtensions();
  instance_info.enabledExtensionCount = static_cast<uint32_t>(sdl_extensions.size());
  instance_info.ppEnabledExtensionNames = sdl_extensions.data();


  VkDebugUtilsMessengerCreateInfoEXT debug_info{};

  if (ValidationLayersEnabled) {
    instance_info.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
    instance_info.ppEnabledExtensionNames = kValidationLayers.data();
    
    FillDebugMessengerCreateInfo(debug_info);
    instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info;
  } else {
    instance_info.enabledLayerCount = 0;
    instance_info.pNext = nullptr;
  }

  if (vkCreateInstance(&instance_info, nullptr, &vk.instance) != VK_SUCCESS) {
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

bool gbengine::GameBoyEngine::VulkanValidationLayerSupported() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* kLayerNames : kValidationLayers) {
    bool layer_found = false;
    for (const auto& layer_properties : available_layers) {
      if (std::strcmp(kLayerNames, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }
    if (!layer_found) {
      return false;
    }
  }
  return true;
}

std::vector<const char*> gbengine::GameBoyEngine::GetExtensions() {
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                        nullptr)) {
    fmt::print(
        "Failed to get the number of Vulkan instance extensions from SDL\n");
    return {};
  }

  std::vector<const char*> extensions(sdl_extension_count);
  if (sdl_extension_count > 0) {
    if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                          extensions.data())) {
      fmt::print("Failed to get the Vulkan instance extensions from SDL\n");
      return {};
    }
  }

  if (ValidationLayersEnabled) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void gbengine::GameBoyEngine::InitVulkanValidationLayers() {

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

