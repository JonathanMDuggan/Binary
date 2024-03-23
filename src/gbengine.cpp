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

// Vulkan Debug functions

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void*                                       pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      spdlog::debug("Validation layer: {}", pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      spdlog::trace("Validation layer: {}", pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      spdlog::warn("Validation layer: {}", pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      spdlog::error("{}\n", pCallbackData->pMessage); break;
    default:
      spdlog::debug("Validation layer: {}", pCallbackData->pMessage); break;
  }

  return VK_FALSE;
}

void gbengine::GameBoyEngine::SetupDebugMessenger() { 
  if (!ValidationLayersEnabled) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  PopulateDebugMessengerCreateInfo(debug_info);

  if (CreateDebugUtilsMessengerEXT(vk.instance, &debug_info ,nullptr, &vk.debug_messenger) != VK_SUCCESS){
    spdlog::critical("Failed to set up debug messenger!");
  }
}


VkResult gbengine::GameBoyEngine::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, 
        "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  return VK_SUCCESS;
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


void gbengine::GameBoyEngine::PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& debug_info) {
  debug_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,

      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,

      .pfnUserCallback = debugCallback,

      .pUserData = nullptr,
  };
}

// Vulkan Instance 

void gbengine::GameBoyEngine::InitVulkanApplication() {

}



void gbengine::GameBoyEngine::InitVulkanInstance() {
  VkInstanceCreateInfo instance_info{};
  if (ValidationLayersEnabled && !VulkanValidationLayerSupported()) {
    spdlog::critical("Validation layers requested, but not available!");
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
    instance_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instance_info.ppEnabledLayerNames = validation_layers.data();
    
    PopulateDebugMessengerCreateInfo(debug_info);
    instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info;
  } else {
    instance_info.enabledLayerCount = 0;
    instance_info.pNext = nullptr;
  }

  if (vkCreateInstance(&instance_info, nullptr, &vk.instance) != VK_SUCCESS) {
    spdlog::critical("Failed to create Vulkan instance");
    return;
  }

  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensions.data());

  spdlog::info("Available extensions:");
  for (const auto& extension : extensions) {
    spdlog::info("  {}", extension.extensionName);
  }
}

bool gbengine::GameBoyEngine::VulkanValidationLayerSupported() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* kLayerNames : validation_layers) {
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

// Vulkan Physical Device Functions

bool gbengine::GameBoyEngine::IsDeviceSuitable(VkPhysicalDevice devices) {
  QueueFamilyIndices indices = FindQueueFamilies(devices);
  return indices.IsComplete();
}

int gbengine::GameBoyEngine::RateDeviceSuitabillity(VkPhysicalDevice device) {
  int score = 0;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures   device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);
  vkGetPhysicalDeviceProperties(device, &device_properties);
  if (!device_features.geometryShader) {
    return 0;
  }
  // We'll take the discete graphics card over any intergrated GPU.
  // Even the worse ones 
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }
  score += device_properties.limits.maxImageDimension2D;
  return score;
}

void gbengine::GameBoyEngine::PickPhsycialDevice() {
  uint32_t device_count = 0;
  std::multimap<int, VkPhysicalDevice> device_candidates;

  vk.physical_device = VK_NULL_HANDLE;
  vkEnumeratePhysicalDevices(vk.instance, &device_count, nullptr);

  if (device_count == 0) spdlog::critical("Failed to find GPU with vulkan support!");

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(vk.instance, &device_count, devices.data());

  for (const auto& physical_device : devices) {
    if (IsDeviceSuitable(physical_device)) {
      vk.physical_device = physical_device;
      break;
    }
  
    if (vk.physical_device == VK_NULL_HANDLE) {
      spdlog::critical("Failed to find a suitable GPU!");
    }
  }
  // Replace the above for loop if you want some sort of score system
  //for (VkPhysicalDevice device : devices) {
  //  int score = RateDeviceSuitabillity(device);
  //  device_candidates.insert(std::make_pair(score, device));
  //}
  //if (device_candidates.rbegin()->first > 0) {
  //  vk.physical_device = device_candidates.rbegin()->second;
  //} else {
  //  fmt::print("Failed to find a suitable GPU!");
  //}
}

void gbengine::GameBoyEngine::CreateLogicalDevice() {
  float queue_priority = 1.0f;
  VkDeviceCreateInfo device_info;
  VkPhysicalDeviceFeatures device_features{};
  QueueFamilyIndices indinces = FindQueueFamilies(vk.physical_device);
  VkDeviceQueueCreateInfo queue_info;

  queue_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = indinces.graphics_family.value(),
    .queueCount = 1,
    .pQueuePriorities = &queue_priority,
  };

  device_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queue_info,
    .pEnabledFeatures = &device_features,
  };

  device_info.enabledExtensionCount = 0;

  if (ValidationLayersEnabled) {
    device_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    device_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    device_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(vk.physical_device, &device_info, nullptr, &vk.device) != VK_SUCCESS) {
    spdlog::critical("Failed to create logical device");
  }

  vkGetDeviceQueue(vk.device, indinces.graphics_family.value(), 0, &vk.queue);
}

// Vulkan Queue


QueueFamilyIndices gbengine::GameBoyEngine::FindQueueFamilies(
    VkPhysicalDevice device) {
  int i = 0;
  uint32_t queue_family_count = 0;
  QueueFamilyIndices indices;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  for (const auto& queue_family: queue_families){
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    if (indices.IsComplete()) {
      break;
    }
    i++;
  }
  return indices;
}


// Vulkan Extensions

std::vector<const char*> gbengine::GameBoyEngine::GetExtensions() {
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                        nullptr)) {
    spdlog::critical(
        "Failed to get the number of Vulkan instance extensions from SDL");
    return {};
  }

  std::vector<const char*> extensions(sdl_extension_count);
  if (sdl_extension_count > 0) {
    if (!SDL_Vulkan_GetInstanceExtensions(sdl.window, &sdl_extension_count,
                                          extensions.data())) {
      spdlog::critical(
          "Failed to get the Vulkan instance extensions from SDL");
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

// SDL Stuff

void gbengine::GameBoyEngine::InitSDL() {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(app_info.name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, config.resolution.width,
                                config.resolution.height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
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
 
 gbengine::GameBoyEngine::GameBoyEngine() {
   app_info.name            = GB_ENGINE_NAME;
   config.resolution.height = kStandardDefinitionHeight;
   config.resolution.width  = kStandardDefinitionWidth;
   InitSDL();
   InitVulkan();
   return;
}

 void gbengine::GameBoyEngine::InitVulkan() {
  if (ValidationLayersEnabled) spdlog::set_level(spdlog::level::debug);
  InitVulkanInstance();
  SetupDebugMessenger();
  PickPhsycialDevice();
  CreateLogicalDevice();
 }

 gbengine::GameBoyEngine::~GameBoyEngine() {

   // If i don't do this, memory corrupts.
   if (vk.device) {
     vkDestroyDevice(vk.device, nullptr);
     vk.device = VK_NULL_HANDLE;
   }

   if (vk.instance) {
     vkDestroyInstance(vk.instance, nullptr);
     vk.instance = VK_NULL_HANDLE;
   }

   SDL_DestroyWindow(sdl.window);
   SDL_Quit();
 }