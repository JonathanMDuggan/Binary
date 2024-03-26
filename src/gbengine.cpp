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
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
   
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
    .applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
    .pEngineName = app_info.name,
    .engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };
  //required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &vk.app_info;

  std::vector<const char*> sdl_extensions = GetExtensions();
  instance_info.enabledExtensionCount = static_cast<uint32_t>(sdl_extensions.size());
  instance_info.ppEnabledExtensionNames = sdl_extensions.data();


  VkDebugUtilsMessengerCreateInfoEXT debug_info{};

  if (ValidationLayersEnabled) {
    instance_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
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
  bool extension_supported = CheckDeviceExtensionSupport(devices);
  bool swap_chain_adequate = false;
  if (extension_supported) {
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(devices);
    swap_chain_adequate = !swap_chain_support.formats.empty() &&
                          !swap_chain_support.present_modes.empty();
  }
  return indices.IsComplete() && extension_supported && swap_chain_adequate;
}

 bool gbengine::GameBoyEngine::CheckDeviceExtensionSupport(
   VkPhysicalDevice device) {
  
   uint32_t extension_count;
   vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                        nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
   vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                        available_extensions.data());
   
  std::set<std::string> required_extensions(device_extensions.begin(),
                                            device_extensions.end());
   for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
   }

   return required_extensions.empty();
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

  if (device_count == 0)
    spdlog::critical("Failed to find GPU with vulkan support!");

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
  std::vector<VkDeviceQueueCreateInfo> queue_create_info;
  std::set<uint32_t> unique_queue_families = {
    indinces.graphics_family.value(),
    indinces.present_family.value()
  };

  for (uint32_t queue_family : unique_queue_families) {
     queue_info = {
       .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
       .queueFamilyIndex = queue_family,
       .queueCount = 1,
       .pQueuePriorities = &queue_priority};
    queue_create_info.push_back(queue_info);
  }

  device_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size()),
    .pQueueCreateInfos = queue_create_info.data(),
    .pEnabledFeatures = &device_features
  };

  device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
  device_info.ppEnabledExtensionNames = device_extensions.data();

  if (ValidationLayersEnabled) {
    device_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    device_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    device_info.enabledLayerCount = 0;
  }
  VkResult result =
      vkCreateDevice(vk.physical_device, &device_info, nullptr, &vk.device);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create logical device");
  } else {
    spdlog::info("vkCreateDevice returned with the following: {}",
                  VkResultToString(result));
  }

  vkGetDeviceQueue(vk.device, indinces.graphics_family.value(), 0, &vk.queue);
  vkGetDeviceQueue(vk.device, indinces.present_family.value(), 0, &vk.queue);
}

// Vulkan Queue


QueueFamilyIndices gbengine::GameBoyEngine::FindQueueFamilies(
    VkPhysicalDevice device) {
  VkBool32 present_support;
  QueueFamilyIndices indices;
  int i = 0;
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  for (const auto& queue_family: queue_families){
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk.surface,
                                         &present_support);
    if (present_support) {
      indices.present_family = i;
    }
  
    if (indices.IsComplete()) break;
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

// Vulkan SwapChain stuff

gbengine::GameBoyEngine::SwapChainSupportDetails
gbengine::GameBoyEngine::QuerySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  uint32_t format_count;
  uint32_t present_mode_count;
  bool swap_chain_adequate = false;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk.surface,
                                            &details.capabilities);

  vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surface,
                                       &format_count, nullptr);

  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surface,
                                         &format_count, details.formats.data());
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk.surface,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk.surface,
                                              &present_mode_count,
                                              details.present_modes.data());
  }
  
  return details;
}

VkSurfaceFormatKHR gbengine::GameBoyEngine::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available_format : available_formats) {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }
  return available_formats[0];
}

VkPresentModeKHR gbengine::GameBoyEngine::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
  for (const VkPresentModeKHR& available_present_mode :
       available_present_modes) {
    if (available_present_mode == VK_PRESENT_MODE_FIFO_KHR) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D gbengine::GameBoyEngine::ChooseSwapExtent(
  const VkSurfaceCapabilitiesKHR& capabilities) {
  int width, height = 0;
  
  SDL_Vulkan_GetDrawableSize(sdl.window, &width, &height);
  
  VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                              static_cast<uint32_t>(height)};
  
  actual_extent.width = std::clamp(actual_extent.width, 
                                   capabilities.minImageExtent.width,
                                   capabilities.maxImageExtent.width);
  
  actual_extent.height = std::clamp(actual_extent.height, 
                                    capabilities.minImageExtent.height,
                                    capabilities.maxImageExtent.height);
  
  return actual_extent;
}
// Started breaking the 80 col rule due to lack of readability

 void gbengine::GameBoyEngine::CreateSwapChain() {
  uint32_t image_count = 0;
  SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(vk.physical_device);
  VkSurfaceFormatKHR surface_format          = ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode              = ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent                          = ChooseSwapExtent(swap_chain_support.capabilities);
  QueueFamilyIndices indices                 = FindQueueFamilies(vk.physical_device);
  VkSwapchainCreateInfoKHR swap_chain_info{};
  const uint32_t queue_family_indices[2] = {indices.graphics_family.value(),
                                            indices.present_family.value()};

  image_count = swap_chain_support.capabilities.minImageCount;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  swap_chain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_info.surface = vk.surface;
  swap_chain_info.minImageCount = swap_chain_support.capabilities.minImageCount;
  swap_chain_info.imageFormat = surface_format.format;
  swap_chain_info.imageColorSpace = surface_format.colorSpace;
  swap_chain_info.imageExtent = extent;
  swap_chain_info.imageArrayLayers = 1;
  swap_chain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (indices.graphics_family != indices.present_family) {
    swap_chain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swap_chain_info.queueFamilyIndexCount = 2;
    swap_chain_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    swap_chain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // swap_chain_info.queueFamilyIndexCount = 0;
    // swap_chain_info.pQueueFamilyIndices   = nullptr;
  }

  swap_chain_info.preTransform = swap_chain_support.capabilities.currentTransform;
  swap_chain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  swap_chain_info.clipped = VK_TRUE;
  // swap_chain_info.oldSwapchain   = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(vk.device, &swap_chain_info, nullptr,
                                         &vk.swap_chain.KHR);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create swap chain! {} on line {} in file {}",
                     VkResultToString(result), __LINE__, __FILE__);
  }

  vkGetSwapchainImagesKHR(vk.device, vk.swap_chain.KHR, &image_count, nullptr);
  vk.swap_chain.images.resize(image_count);
  vkGetSwapchainImagesKHR(vk.device, vk.swap_chain.KHR, &image_count,
                          vk.swap_chain.images.data());

  vk.swap_chain.image_format = surface_format.format;
  vk.swap_chain.extent = extent;
}

// Vulkan Image Views

 void gbengine::GameBoyEngine::CreateImageViews() {
   VkImageViewCreateInfo image_view_info{};
   vk.swap_chain.image_views.resize(vk.swap_chain.images.size());
   for (size_t i = 0; i < vk.swap_chain.images.size(); i++) {
     image_view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
     image_view_info.image                           = vk.swap_chain.images[i];
     image_view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
     image_view_info.format                          = vk.swap_chain.image_format;
     image_view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
     image_view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
     image_view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
     image_view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
     image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
     image_view_info.subresourceRange.baseMipLevel   = 0;
     image_view_info.subresourceRange.levelCount     = 1;
     image_view_info.subresourceRange.baseArrayLayer = 0;
     image_view_info.subresourceRange.layerCount     = 1;
   
     VkResult result = vkCreateImageView(vk.device, &image_view_info, nullptr,
                                         &vk.swap_chain.image_views[i]);
     if (result != VK_SUCCESS) {
       spdlog::error("Failed to create image view! {} on line {} in file {}",
                        VkResultToString(result), __LINE__, __FILE__);
     }
   }
 }

// Vulkan Graphics Pipeline
 void gbengine::GameBoyEngine::CreateGraphicsPipeline() { 
   spdlog::info("Creating Graphics Pipeline");
   
 }
 // SDL Stuff

void gbengine::GameBoyEngine::InitSDL() {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(app_info.name,
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, 
                                config.resolution.width,
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
    if (ValidationLayersEnabled) spdlog::set_level(spdlog::level::trace);
    InitVulkanInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhsycialDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateImageViews();
 }

 void gbengine::GameBoyEngine::CreateSurface() {
   if (SDL_Vulkan_CreateSurface(sdl.window, vk.instance, &vk.surface)) {
     return;
   }
 }


 gbengine::GameBoyEngine::~GameBoyEngine() {
   VkDebugUtilsMessengerEXT debug_info{};
   // If i don't do this, memory corrupts.

   for (auto image_view: vk.swap_chain.image_views) {
     vkDestroyImageView(vk.device, image_view, nullptr);
   }

   if (vk.swap_chain.KHR != VK_NULL_HANDLE) {
     vkDestroySwapchainKHR(vk.device, vk.swap_chain.KHR, nullptr);
     vk.swap_chain.KHR = VK_NULL_HANDLE;
   }
   
   if (vk.device) {
     vkDestroyDevice(vk.device, nullptr);
     vk.device = VK_NULL_HANDLE;
   }
   if (vk.surface) {
     vkDestroySurfaceKHR(vk.instance, vk.surface, nullptr);
     vk.surface = VK_NULL_HANDLE;
   }
   if (ValidationLayersEnabled) {
     DestroyDebugUtilsMessengerEXT(vk.instance, vk.debug_messenger, nullptr);
   }

   if (vk.instance) {
     vkDestroyInstance(vk.instance, nullptr);
     vk.instance = VK_NULL_HANDLE;
   }

   SDL_DestroyWindow(sdl.window);
   SDL_Quit();
 }

 std::string gbengine::VkResultToString(VkResult result) {
   switch (result) {
     case VK_SUCCESS:
       return "VK_SUCCESS";
     case VK_NOT_READY:
       return "VK_NOT_READY";
     case VK_TIMEOUT:
       return "VK_TIMEOUT";
     case VK_EVENT_SET:
       return "VK_EVENT_SET";
     case VK_EVENT_RESET:
       return "VK_EVENT_RESET";
     case VK_INCOMPLETE:
       return "VK_INCOMPLETE";
     case VK_ERROR_OUT_OF_HOST_MEMORY:
       return "VK_ERROR_OUT_OF_HOST_MEMORY";
     case VK_ERROR_OUT_OF_DEVICE_MEMORY:
       return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
     case VK_ERROR_INITIALIZATION_FAILED:
       return "VK_ERROR_INITIALIZATION_FAILED";
     case VK_ERROR_DEVICE_LOST:
       return "VK_ERROR_DEVICE_LOST";
     case VK_ERROR_MEMORY_MAP_FAILED:
       return "VK_ERROR_MEMORY_MAP_FAILED";
     case VK_ERROR_LAYER_NOT_PRESENT:
       return "VK_ERROR_LAYER_NOT_PRESENT";
     case VK_ERROR_EXTENSION_NOT_PRESENT:
       return "VK_ERROR_EXTENSION_NOT_PRESENT";
     case VK_ERROR_FEATURE_NOT_PRESENT:
       return "VK_ERROR_FEATURE_NOT_PRESENT";
     case VK_ERROR_INCOMPATIBLE_DRIVER:
       return "VK_ERROR_INCOMPATIBLE_DRIVER";
     case VK_ERROR_TOO_MANY_OBJECTS:
       return "VK_ERROR_TOO_MANY_OBJECTS";
     case VK_ERROR_FORMAT_NOT_SUPPORTED:
       return "VK_ERROR_FORMAT_NOT_SUPPORTED";
     case VK_ERROR_FRAGMENTED_POOL:
       return "VK_ERROR_FRAGMENTED_POOL";
     case VK_ERROR_UNKNOWN:
       return "VK_ERROR_UNKNOWN";
     case VK_ERROR_OUT_OF_POOL_MEMORY:
       return "VK_ERROR_OUT_OF_POOL_MEMORY";
     case VK_ERROR_INVALID_EXTERNAL_HANDLE:
       return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
     case VK_ERROR_FRAGMENTATION:
       return "VK_ERROR_FRAGMENTATION";
     case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
       return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
     case VK_PIPELINE_COMPILE_REQUIRED:
       return "VK_PIPELINE_COMPILE_REQUIRED";
     case VK_ERROR_SURFACE_LOST_KHR:
       return "VK_ERROR_SURFACE_LOST_KHR";
     case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
       return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
     case VK_SUBOPTIMAL_KHR:
       return "VK_SUBOPTIMAL_KHR";
     case VK_ERROR_OUT_OF_DATE_KHR:
       return "VK_ERROR_OUT_OF_DATE_KHR";
     case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
       return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
     case VK_ERROR_VALIDATION_FAILED_EXT:
       return "VK_ERROR_VALIDATION_FAILED_EXT";
     case VK_ERROR_INVALID_SHADER_NV:
       return "VK_ERROR_INVALID_SHADER_NV";
     case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
       return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
       return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
       return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
       return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
       return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
       return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
     case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
       return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
     case VK_ERROR_NOT_PERMITTED_KHR:
       return "VK_ERROR_NOT_PERMITTED_KHR";
     case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
       return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
     case VK_THREAD_IDLE_KHR:
       return "VK_THREAD_IDLE_KHR";
     case VK_THREAD_DONE_KHR:
       return "VK_THREAD_DONE_KHR";
     case VK_OPERATION_DEFERRED_KHR:
       return "VK_OPERATION_DEFERRED_KHR";
     case VK_OPERATION_NOT_DEFERRED_KHR:
       return "VK_OPERATION_NOT_DEFERRED_KHR";
     case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
       return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
     case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
       return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
     case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
       return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
     default:
       return "Unknown Vulkan error code";
   }
 }