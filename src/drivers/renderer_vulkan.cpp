// File: renderer_vulkan.cpp
// Programmer: Jonathan M Duggan
// Date: 2024-03-14
// Purpose: Gameboy Engine start up file
// Version: 0.0.
//
// NOTE: This file is heavliy commented becasue I don't know what im writing
//       I'm following a guide online called https://vkguide.dev/. Vulkan is
//       Complex, I need explanation.

#pragma once
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#define VMA_VULKAN_VERSION 1002000
#include "include/renderer_vulkan.h"
#include "include/peripherals_sdl.h"


void gbengine::Vulkan::InitVulkan(SDL* sdl, Application app) {
  if (ValidationLayersEnabled) spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing Vulkan Instance");
  InitVulkanInstance(sdl->window_, app);
  SetupDebugMessenger();
  CreateSurface(sdl->window_);
  spdlog::info("Finding a suitable device that supports Vulkan");
  PickPhysicalDevice();
  spdlog::info("Initializing Vulkan Logical Device");
  CreateLogicalDevice();
  spdlog::info("Initializing Vulkan Presentation Layer");
  CreateSwapChain(sdl->window_);
  CreateImageViews();
  spdlog::info("Creating Vulkan Graphics Pipeline");
  CreateRenderPass();
  CreateDescriptorSetLayout();
  CreateGraphicsPipeline();
  spdlog::info("Allocating Vulkan Buffers");
  CreateCommandPool();
  CreateDepthResources();
  CreateFrameBuffer(); 
  CreateTextureImage(sdl);
  CreateTextureImageView();
  CreateTextureSampler();
  CreateVertexBuffer(); 
  CreateIndexBuffer();
  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();
  CreateCommandBuffer();
  CreateSyncObjects();
}

// Vulkan Debug functions

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      spdlog::debug("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      spdlog::trace("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      spdlog::warn("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      spdlog::error("{}\n", pCallbackData->pMessage);
      break;
    default:
      spdlog::debug("Validation layer: {}", pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}

void gbengine::Vulkan::SetupDebugMessenger() {
  if (!ValidationLayersEnabled) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  PopulateDebugMessengerCreateInfo(debug_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr,
                                  &debug_messenger_) != VK_SUCCESS) {
    spdlog::critical("Failed to set up debug messenger!");
  }
}

VkResult gbengine::Vulkan::CreateDebugUtilsMessengerEXT(
    VkInstance instance_, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance_, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  return VK_SUCCESS;
}



void gbengine::Vulkan::DestroyDebugUtilsMessengerEXT(
    VkInstance instance_, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance_, "vkDestroyDebugUtilsMessengerEXT");

  if (func != nullptr) {
    func(instance_, debugMessenger, pAllocator);
  }
}



void gbengine::Vulkan::PopulateDebugMessengerCreateInfo(
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

// Creates a "Vulkan Instance" taking two parameters:
// Window: What window will Vulkan renender to
// Application: The application Vulkan is under.
void gbengine::Vulkan::InitVulkanInstance(SDL_Window* window_, Application app) {
  // The Vulkan Instance sits at the top of the Vulkan hierachy, the vulkan
  // instance must be initizted with what appliaction infomation ( name,
  // version, engine etc. while also requiring the extensions of outside
  // libraies like SDL, GLSW, or other window libaries inorder for vulkan 
  // to render to a window.
  VkInstanceCreateInfo instance_info{};
  VkApplicationInfo app_info{};
  VkResult result;

  // Checks if validation layers are supported in this version of vulkan
  // only prints to the console if the program was compiled in debug mode
  if (ValidationLayersEnabled && !VulkanValidationLayerSupported()) {
    spdlog::critical("Validation layers requested, but not available!");
  }

  // Init the application infomation to Vulkan
  app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = app.name,
      .applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
      .pEngineName = app.name,
      .engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };
  // required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  // Vulkan Instances can have what are called "extentions". Extensions are
  // features which allow Vulkan to do more things it otherwise cannot do.
  // In this line of code, we are calling the get extension function
  // which will get the extensions needed to use SDL for the windowing 
  // and the debug extenstion for the validation layers
  std::vector<const char*> sdl_extensions = GetExtensions(window_);
  instance_info.enabledExtensionCount =
      static_cast<uint32_t>(sdl_extensions.size());
  instance_info.ppEnabledExtensionNames = sdl_extensions.data();

  // If Validation layers were enabled, populate the debug infomation struct
  // and add validation layer data to the instance info.
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

  // Create the vulkan instance with all the infomation given above
  result = vkCreateInstance(&instance_info, nullptr, &instance_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Vulkan instance");
    std::runtime_error("Failed to create Vulkan instance");
    return;
  }

  // Collect the infomation from the vulkan instance, and print the extensions
  // to the terminal
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

bool gbengine::Vulkan::VulkanValidationLayerSupported() {
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

bool gbengine::Vulkan::IsPhysicalDeviceSuitable(
    VkPhysicalDevice phyiscal_device) {
  // Not every device which supports Vulkan can display images to the screen
  // therefore we must check if the physical device is suitable for the
  // application needs
  QueueFamilyIndices indices = FindQueueFamilies(phyiscal_device);
  bool extension_supported = CheckDeviceExtensionSupport(phyiscal_device);
  bool swap_chain_adequate = false;
  // Check if the physical device has the required extensions
  if (extension_supported) {
    // Check if the physical device can utilize swap chains
    SwapChainSupportDetails swap_chain_support =
        QuerySwapChainSupport(phyiscal_device);
    swap_chain_adequate = !swap_chain_support.formats.empty() &&
                          !swap_chain_support.present_modes.empty();
  }
  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(phyiscal_device, &supported_features);
  // If the physical device and support all of these things return 1
  return indices.IsComplete() && extension_supported && swap_chain_adequate && 
         supported_features.samplerAnisotropy; 
}



bool gbengine::Vulkan::CheckDeviceExtensionSupport(
    VkPhysicalDevice physical_device) {
  uint32_t extension_count;
  // The application requires certain extensions. When we created the vulkan
  // instance we specified what extensions the are needed for the instance.
  // The Physical device also need to support these extensions.
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &extension_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &extension_count, available_extensions.data());

  // check if the physical has the required extensions needed for the application
  // to work
  std::set<std::string> required_extensions(device_extensions.begin(),
                                            device_extensions.end());
  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  // returns 1 if the physical device supports all the required extensions 
  return required_extensions.empty();
}

void gbengine::Vulkan::CreateUniformBuffers() {
  VkDeviceSize buffer_size = sizeof(UniformBufferObject);
  buffer_.uniform_.resize(kMaxFramesInFlight);
  buffer_.uniform_memory_.resize(kMaxFramesInFlight);
  buffer_.uniform_mapped_.resize(kMaxFramesInFlight);

  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer_.uniform_[i], buffer_.uniform_memory_[i]);

    vkMapMemory(logical_device_, buffer_.uniform_memory_[i], 0, buffer_size, 0,
                &buffer_.uniform_mapped_[i]);
  }
}

int gbengine::Vulkan::RateDeviceSuitabillity(VkPhysicalDevice phyiscal_device) {
  int score = 0;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(phyiscal_device, &device_features);
  vkGetPhysicalDeviceProperties(phyiscal_device, &device_properties);
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

// Finds a suitble device that supports Vulkan
void gbengine::Vulkan::PickPhysicalDevice() {
  // In Vulkan the programmer must choose the physical device in which vulkan
  // will use to render to the screen
  uint32_t device_count = 0;
  // Searches the host computer for a device that supports vulkan
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  // If there is no device in which vulkan cannot support, there's no point
  // in going on. Throw a runtime error!
  if (device_count == 0) {
    spdlog::critical("Failed to find GPU with vulkan support!");
    throw std::runtime_error("");
  }

  // Fill vector with the devices found by vulkan
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  // Not every device which supports vulkan can show images to the screen
  // enumerate over the them and keep the ones which have the capability
  // to display images ( has a graphics queue )
  for (const auto& device_ : devices) {
    if (IsPhysicalDeviceSuitable(device_)) {
      physical_device_ = device_;
      break;
    }
  }

  // If there are no physical devices that can support the application needs
  // Runtime Error!
  if (physical_device_ == VK_NULL_HANDLE) {
    spdlog::critical("Failed to find a suitable GPU!");
    throw std::runtime_error("");
  }
}


void gbengine::Vulkan::CreateLogicalDevice() {
  // A logical device is a Vulkan Concept where parts of the physical device is
  // allocated for the logical device. The logical device is not the hardware
  // it's an abstraction of resources from the physical device that we can use
  // for the application 
  float queue_priority = 1.0f;
  VkDeviceCreateInfo device_info{};
  VkPhysicalDeviceFeatures device_features{};
  // Previous functions already checked if the current physical device have the 
  // Queue families needed for the application, we're calling this to set the
  // logical device info
  QueueFamilyIndices indinces = FindQueueFamilies(physical_device_);
  VkDeviceQueueCreateInfo queue_info{};

  std::vector<VkDeviceQueueCreateInfo> queue_create_info;
  std::set<uint32_t> unique_queue_families = {indinces.graphics_family.value(),
                                              indinces.present_family.value()};

  for (uint32_t queue_family : unique_queue_families) {
    queue_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                  .queueFamilyIndex = queue_family,
                  .queueCount = 1,
                  .pQueuePriorities = &queue_priority};
    queue_create_info.push_back(queue_info);
  }

  // Match the logical device with the queue index of the physical device
  device_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size()),
      .pQueueCreateInfos = queue_create_info.data(),
      .pEnabledFeatures = &device_features};

  // Add the extensions needed for the application for the logical device
  device_info.enabledExtensionCount =
      static_cast<uint32_t>(device_extensions.size());
  device_info.ppEnabledExtensionNames = device_extensions.data();

  // Add debuging capabilities for the logical device if validation layers were
  // enabled
  if (ValidationLayersEnabled) {
    device_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    device_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    device_info.enabledLayerCount = 0;
  }

  VkResult result =
      vkCreateDevice(physical_device_, &device_info, nullptr, &logical_device_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create logical device");
  } else {
    spdlog::info("vkCreateDevice returned with the following: {}",
                 VkResultToString(result));
  }

  // The physical device queue location is in a index, to get the queue we
  // must set the pass the index to this function to fetch the queue
  vkGetDeviceQueue(logical_device_, indinces.graphics_family.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(logical_device_, indinces.present_family.value(), 0,
                   &present_queue_);
}

// Vulkan Queue

// Gets queue familes from the phyiscal device
gbengine::QueueFamilyIndices gbengine::Vulkan::FindQueueFamilies(
    VkPhysicalDevice phyiscal_device) {
  VkBool32 present_support;
  QueueFamilyIndices indices;
  int i = 0;
  uint32_t queue_family_count = 0;
  // Get the queue familes from the physical device so we can determind if the
  // physical device and display graphics to the screen.
  vkGetPhysicalDeviceQueueFamilyProperties(phyiscal_device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(phyiscal_device, &queue_family_count,
                                           queue_families.data());

  
  for (const auto& queue_family : queue_families) {
    // Checks if the physical device can display graphics to the screen
    // If it does set graphic family to i.
    // 
    // NOTE: graphic_family is a std::optional, meaning it holds nothing
    // if it isn't set, this is okay since we can use the method .has_value()
    // to check if the grpahic_family has a value.
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    // Even if the queue family supports graphics, that doesn't mean it 
    // supports presenting those graphics to the host machine. Check if
    // the queue family has present support
    present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(phyiscal_device, i, surface_,
                                         &present_support);
    if (present_support) {
      indices.present_family = i;
    }

    // If the queue family supports both graphics and presenting, break the
    // for loop and return with the indices.
    if (indices.IsComplete()) break;
    i++;
  }
  return indices;
}


// Vulkan Extensions

// Gets extensions from the SDL library to allow vulkan to do features it
// otherwise cannot do by itself
std::vector<const char*> gbengine::Vulkan::GetExtensions(
    SDL_Window* window_) {

  // Get the number of extensions sdl needs and increase the size of a vector
  // based on the number it gives, then push the extension names inside of
  // vector
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window_, &sdl_extension_count,
                                        nullptr)) {
    spdlog::critical(
        "Failed to get the number of Vulkan instance extensions from SDL");
    return {};
  }

  std::vector<const char*> extensions(sdl_extension_count);
  if (sdl_extension_count > 0) {
    if (!SDL_Vulkan_GetInstanceExtensions(window_, &sdl_extension_count,
                                          extensions.data())) {
      spdlog::critical("Failed to get the Vulkan instance extensions names "
                       "from SDL");
      return {};
    }
  }

  // Adds the debug extensions if validation layers were enabled
  if (ValidationLayersEnabled) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}
// Vulkan SwapChain stuff

// Checks if the physical device supports the khronos groups swap chain
gbengine::Vulkan::SwapChainSupportDetails
gbengine::Vulkan::QuerySwapChainSupport(VkPhysicalDevice phyiscal_device) {
  SwapChainSupportDetails details;
  uint32_t format_count;
  uint32_t present_mode_count;
  bool swap_chain_adequate = false;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyiscal_device, surface_,
                                            &details.capabilities);

  vkGetPhysicalDeviceSurfaceFormatsKHR(phyiscal_device, surface_, &format_count,
                                       nullptr);

  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phyiscal_device, surface_,
                                         &format_count,
                                         details.formats.data());
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(phyiscal_device, surface_,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phyiscal_device, surface_,
                                              &present_mode_count,
                                              details.present_modes.data());
  }

  return details;
}

// Returns the supported image format for the swap chain
VkSurfaceFormatKHR gbengine::Vulkan::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available_format : available_formats) {
    if (available_format.format == VK_FORMAT_R8G8B8_SRGB &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }
  return available_formats[0];
}

VkPresentModeKHR gbengine::Vulkan::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
  for (const VkPresentModeKHR& available_present_mode :
       available_present_modes) {
    if (available_present_mode == VK_PRESENT_MODE_FIFO_KHR) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}



VkExtent2D gbengine::Vulkan::ChooseSwapExtent(
    SDL_Window* window_,
    const VkSurfaceCapabilitiesKHR& capabilities) {
  int width, height = 0;

  SDL_Vulkan_GetDrawableSize(window_, &width, &height);

  VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                              static_cast<uint32_t>(height)};

  actual_extent.width =
      std::clamp(actual_extent.width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);

  actual_extent.height =
      std::clamp(actual_extent.height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  return actual_extent;
}

VkFormat gbengine::Vulkan::FindSupportedFormat(
    const std::vector<VkFormat>& candidates, VkImageTiling tiling,
    VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(physical_device_, format, &properties);
    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (properties.linearTilingFeatures & features) == features) {
      return format;  
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
        (properties.optimalTilingFeatures & features) == features);
    return format;  
  }
  spdlog::critical("Failed to find supported format for tiling");
  throw std::runtime_error("Failed to find supported format for tiling");
}
VkFormat gbengine::Vulkan::FindDepthFormat() {
  return FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  
}
bool gbengine::Vulkan::HasStenceilComponent(VkFormat format) { 
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void gbengine::Vulkan::CreateDepthResources() {
  VkFormat depth_format = FindDepthFormat();
  CreateImage(
      swap_chain_.extent_.width, swap_chain_.extent_.height, depth_format, 
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image_, depth_image_memory_);
  depth_image_view_ = 
      CreateImageView(depth_image_, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void gbengine::Vulkan::CreateSwapChain(SDL_Window* window_) {
  uint32_t image_count = 0;
  SwapChainSupportDetails swap_chain_support =
      QuerySwapChainSupport(physical_device_);
  VkSurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode =
      ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent = ChooseSwapExtent(window_, swap_chain_support.capabilities);
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  VkSwapchainCreateInfoKHR swap_chain_info{};
  const uint32_t queue_family_indices[2] = {indices.graphics_family.value(),
                                            indices.present_family.value()};

  image_count = swap_chain_support.capabilities.minImageCount;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  swap_chain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_info.surface = surface_;
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

  swap_chain_info.preTransform =
      swap_chain_support.capabilities.currentTransform;
  swap_chain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  swap_chain_info.clipped = VK_TRUE;
  // swap_chain_info.oldSwapchain   = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(logical_device_, &swap_chain_info,
                                         nullptr, &swap_chain_.KHR_);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create swap chain! {} on line {} in file {}",
                     VkResultToString(result), __LINE__, __FILE__);
  }

  vkGetSwapchainImagesKHR(logical_device_, swap_chain_.KHR_, &image_count,
                          nullptr);
  swap_chain_.images_.resize(image_count);
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_.KHR_, &image_count,
                          swap_chain_.images_.data());

  swap_chain_.image_format_ = surface_format.format;
  swap_chain_.extent_ = extent;
}

void gbengine::Vulkan::RecreateSwapChain(SDL_Window* window_, SDL_Event* event) {
  int width = 0;
  int height = 0;
  SDL_Vulkan_GetDrawableSize(window_, &width, &height);
  while (width == 0 || height == 0) {
    SDL_Vulkan_GetDrawableSize(window_, &width, &height);
    SDL_WaitEvent(event);
  }
  vkDeviceWaitIdle(logical_device_);
  CleanUpSwapChain();
  CreateSwapChain(window_);
  CreateImageViews();
  CreateDepthResources();
  CreateFrameBuffer();
}

void gbengine::Vulkan::CleanUpSwapChain() {

  vkDestroyImageView(logical_device_, depth_image_view_, nullptr);
  depth_image_view_ = VK_NULL_HANDLE;
  vkDestroyImage(logical_device_, depth_image_, nullptr);
  depth_image_ = VK_NULL_HANDLE; 
  vkFreeMemory(logical_device_, depth_image_memory_, nullptr);
  depth_image_memory_ = VK_NULL_HANDLE; 
  for (uint32_t i = 0; i < swap_chain_.frame_buffer_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_.frame_buffer_[i],
                         nullptr);
  }
  for (uint32_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_.image_views_[i], nullptr);
  }
  vkDestroySwapchainKHR(logical_device_, swap_chain_.KHR_, nullptr);
  vkFreeMemory(logical_device_, depth_image_memory_, nullptr);
}
void gbengine::Vulkan::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2>pool_size{};
  VkDescriptorPoolCreateInfo pool_info{};  
  std::array<VkWriteDescriptorSet, 2> descriptor_writes{}; 
  VkResult result;

  pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  
  pool_size[0].descriptorCount = static_cast<uint32_t>(kMaxFramesInFlight);  
  pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;  
  pool_size[1].descriptorCount = static_cast<uint32_t>(kMaxFramesInFlight);   

  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; 
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_size.size()); 
  pool_info.pPoolSizes = pool_size.data(); 
  pool_info.maxSets = static_cast<uint32_t>(kMaxFramesInFlight);  

  result = vkCreateDescriptorPool(logical_device_, &pool_info, nullptr,
                                  &descriptor_pool_);
  if (result != VK_SUCCESS) {
    spdlog::critical(
        "Failed to create Desciptor Pool! {} on line {} in file {}",
        VkResultToString(result), __LINE__, __FILE__);
    throw std::runtime_error("Failed to create Desciptor Pool!: " +
                             VkResultToString(result));
  }
}
// Vulkan Image Views

void gbengine::Vulkan::CreateImageViews() {

  swap_chain_.image_views_.resize(swap_chain_.images_.size());
  for (size_t i = 0; i < swap_chain_.images_.size(); i++) {
    swap_chain_.image_views_[i] =
        CreateImageView(swap_chain_.images_[i], swap_chain_.image_format_,
                        VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

// Vulkan Graphics Pipeline

VkShaderModule gbengine::Vulkan::CreateShaderModule(
    const std::vector<char>& code) {
  VkShaderModuleCreateInfo shader_module_info{};
  shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_info.codeSize = code.size();
  shader_module_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  VkResult result = vkCreateShaderModule(logical_device_, &shader_module_info,
                                         nullptr, &shader_module);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create shader module: {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create shader module");
  }

  return shader_module;
}

// Vulkan Graphics Pipeline

void gbengine::Vulkan::CreateGraphicsPipeline() {
  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  VkPipelineShaderStageCreateInfo shader_stages[2];
  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDepthStencilStateCreateInfo depth_stencil{}; 
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  VkViewport viewport{};
  VkRect2D scissor{};
  VkPipelineViewportStateCreateInfo viewport_state{};
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  VkPipelineMultisampleStateCreateInfo multisampling{};
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  VkPipelineColorBlendStateCreateInfo color_blending{};
  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  VkGraphicsPipelineCreateInfo pipeline_info{};
  // Get the shaders and store them into local memory
  auto vert_shader_code = ReadFile("shaders/vert.spv");
  auto frag_shader_code = ReadFile("shaders/frag.spv");
  VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);
  auto binding_descriptions = Vertex::GetBindingDescription();
  auto attribute_descriptions = Vertex::GetAttributeDesciptions();

  vert_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";

  frag_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";

  shader_stages[0] = vert_shader_stage_info;
  shader_stages[1] = frag_shader_stage_info;

  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount =
          static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexBindingDescriptions = &binding_descriptions;
  vertex_input_info.pVertexAttributeDescriptions = 
      attribute_descriptions.data(); 

  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  scissor.offset = {0, 0};
  scissor.extent = swap_chain_.extent_;

  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount =
      static_cast<uint32_t>(dynamic_states.size());
  dynamic_state.pDynamicStates = dynamic_states.data();

  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;

  // Rasterizer transforms our triangle into discrete pixels for the screen. 

  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.stencilTestEnable = VK_FALSE;

  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
  pipeline_layout_info.pushConstantRangeCount = 0;

  VkResult result = vkCreatePipelineLayout(
      logical_device_, &pipeline_layout_info, nullptr, &pipeline_layout_
  );
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create pipeline layout! {} ",
                     VkResultToString(result));
    throw std::runtime_error("");
  }

  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_state;
  pipeline_info.pDepthStencilState = &depth_stencil;
  pipeline_info.layout = pipeline_layout_;
  pipeline_info.renderPass = render_pass_;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  result = vkCreateGraphicsPipelines(logical_device_, VK_NULL_HANDLE, 1,
                                &pipeline_info, nullptr, &graphics_pipeline_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create graphics pipeline {}",
                     VkResultToString(result));
    throw std::runtime_error("");
  }
  vkDestroyShaderModule(logical_device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(logical_device_, frag_shader_module, nullptr);
}

// Vulkan Render Pass

void gbengine::Vulkan::CreateRenderPass() {
  VkAttachmentDescription color_attachment{};
  VkAttachmentReference color_attachment_reference{};
  VkSubpassDescription subpass{};
  VkRenderPassCreateInfo render_pass_info{};
  VkSubpassDependency dependency{};               
  VkAttachmentDescription depth_attachment{};
  VkAttachmentReference depth_attachment_reference{}; 
  std::array<VkAttachmentDescription, 2> attachments{};
  color_attachment.format = swap_chain_.image_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_reference;
  subpass.pDepthStencilAttachment = &depth_attachment_reference;

  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;                                            
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;                                            
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  depth_attachment.format = FindDepthFormat();
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  depth_attachment_reference.attachment = 1;
  depth_attachment_reference.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  attachments = {color_attachment, depth_attachment};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;       
  render_pass_info.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass(logical_device_, &render_pass_info,
                                       nullptr, &render_pass_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Render Pass!: {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create Render Pass!:" +
                             VkResultToString(result));
  }
}

// Vulkan Frame Buffer

void gbengine::Vulkan::CreateFrameBuffer() {
  VkResult result;
  swap_chain_.frame_buffer_.resize(swap_chain_.image_views_.size());
  for (size_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    std::array<VkImageView, 2> attachments = {swap_chain_.image_views_[i],
                                              depth_image_view_};
    VkFramebufferCreateInfo frame_buffer_info{};
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_info.renderPass = render_pass_;
    frame_buffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    frame_buffer_info.pAttachments = attachments.data();
    frame_buffer_info.width = swap_chain_.extent_.width;
    frame_buffer_info.height = swap_chain_.extent_.height;
    frame_buffer_info.layers = 1;

    result = vkCreateFramebuffer(logical_device_, &frame_buffer_info, nullptr,
                                 &swap_chain_.frame_buffer_[i]);
    if (result != VK_SUCCESS) {
      spdlog::critical("Failed to create Frame Buffer!: {}",
                       VkResultToString(result));
      throw std::runtime_error("");
    }
  }
}

// Vulkan Command Stuff

void gbengine::Vulkan::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
  VkResult result =
      vkCreateCommandPool(logical_device_, &pool_info, nullptr, &command_pool_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Command Pool! {}",
                     VkResultToString(result));
    throw std::runtime_error("");
  }
}

void gbengine::Vulkan::CreateCommandBuffer() {
  command_buffers_.resize(kMaxFramesInFlight);
  VkCommandBufferAllocateInfo allocate_info{};
  VkResult result;
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount = command_buffers_.size();
  result = vkAllocateCommandBuffers(logical_device_, &allocate_info,
                                    command_buffers_.data());
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create Command Buffer! " +
                             VkResultToString(result));
  }
}

void gbengine::Vulkan::RecordCommandBuffer(VkCommandBuffer command_buffer,
                                           uint32_t image_index) {
  VkCommandBufferBeginInfo begin_info{};
  VkResult result;
  VkRenderPassBeginInfo render_pass_info{};
  VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0}}};
  VkViewport viewport{};
  VkRect2D scissor{};
  VkDeviceSize offsets[] = {0};
  VkBuffer vertex_buffers[] = {buffer_.vertex_};
  std::array<VkClearValue, 2> clear_values{};
  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = 0;
  begin_info.pInheritanceInfo = nullptr;
  result = vkBeginCommandBuffer(command_buffer, &begin_info);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to begin recording Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to begin recording command buffer!" +
                             VkResultToString(result));
  }

  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_;
  render_pass_info.framebuffer = swap_chain_.frame_buffer_[image_index];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swap_chain_.extent_;

  clear_values[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};

  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data(); 

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphics_pipeline_);
    // View Port
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain_.extent_.width);
    viewport.height = static_cast<float>(swap_chain_.extent_.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    // Scissor
    scissor.offset = {0,0};
    scissor.extent = swap_chain_.extent_;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    // Vertex Buffer
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    // Index Buffer
    vkCmdBindIndexBuffer(command_buffer, buffer_.index_, 0,
                         VK_INDEX_TYPE_UINT16);
    // Bind Descriptors Sets
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout_, 0, 1,
                            &descriptor_sets_[current_frame_], 0, nullptr);
    // Draw Indexed
    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices_.size()), 1,
                     0, 0, 0);
    //vkCmdDraw(command_buffer, static_cast<uint32_t>(vertices_.size()), 1, 0, 0);
  vkCmdEndRenderPass(command_buffer);
  result = vkEndCommandBuffer(command_buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to record Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to record command buffer!" +
                             VkResultToString(result));
  }
}

void gbengine::Vulkan::CreateIndexBuffer() {
  VkDeviceSize buffer_size = sizeof(indices_[0]) * indices_.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  void * data;
  CreateBuffer(
    buffer_size, 
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,           
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    staging_buffer, staging_buffer_memory);

  vkMapMemory(logical_device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, indices_.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer_memory);
  CreateBuffer(
      buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_.index_,
      buffer_.index_memory_);
  CopyBuffer(staging_buffer, buffer_.index_, buffer_size);

  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);
}

void gbengine::Vulkan::CreateVertexBuffer() { 
  VkDeviceSize buffer_size = sizeof(vertices_[0]) * vertices_.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  void* data;

  CreateBuffer(
    buffer_size, 
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    staging_buffer, staging_buffer_memory
  );

  vkMapMemory(logical_device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vertices_.data(), (size_t)buffer_size); 
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  CreateBuffer(
    buffer_size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_.vertex_, buffer_.vertex_memory_
  );

  CopyBuffer(staging_buffer, buffer_.vertex_, buffer_size);
  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);
}

void gbengine::Vulkan::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding ubo_layout_binding{};
  VkDescriptorSetLayoutCreateInfo layout_info{};
  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
  
  VkDescriptorPoolCreateInfo pool_info{};
  VkResult result;
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; 
  ubo_layout_binding.pImmutableSamplers = nullptr;

  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = nullptr;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  bindings = {ubo_layout_binding, sampler_layout_binding}; 

  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_info.pBindings = bindings.data();
  result = vkCreateDescriptorSetLayout(logical_device_, &layout_info, nullptr,
                                       &descriptor_set_layout_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create descriptor set layout! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to create descriptor set layout!" +
                             VkResultToString(result));
  }
 
  
}

void gbengine::Vulkan::CreateDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layout(kMaxFramesInFlight,
                                            descriptor_set_layout_);
  std::array<VkWriteDescriptorSet, 2> descriptor_writes{}; 
  VkDescriptorSetAllocateInfo allocate_info{};
  VkResult result;
  allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool = descriptor_pool_;
  allocate_info.descriptorSetCount = static_cast<uint32_t>(kMaxFramesInFlight);
  allocate_info.pSetLayouts = layout.data();
  descriptor_sets_.resize(kMaxFramesInFlight);

  result = vkAllocateDescriptorSets(logical_device_, &allocate_info, 
                                    descriptor_sets_.data()); 
  if (result != VK_SUCCESS) { 
    spdlog::critical("Failed to create descriptor set!: {}", 
                     VkResultToString(result)); 
    throw std::runtime_error("Failed to create descriptor set! " +
                             VkResultToString(result)); 
  }

  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    VkDescriptorBufferInfo buffer_info{}; 
    VkDescriptorImageInfo image_info{}; 
    buffer_info.buffer = buffer_.uniform_[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformBufferObject);

    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture_image_view_;
    image_info.sampler = texture_sampler_;

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_sets_[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = descriptor_sets_[i];
    descriptor_writes[1].dstBinding = 1; 
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(logical_device_,
                           static_cast<uint32_t>(descriptor_writes.size()), 
                           descriptor_writes.data(), 0, nullptr); 
  }
}



void gbengine::Vulkan::CreateTextureImage(SDL* sdl) {
  VkBuffer staging_buffer;
  VkDeviceSize image_size;
  VkDeviceMemory staging_buffer_memory;
  uint32_t width, height, format;
  void* data;
  VkResult result;

  sdl->InitSurfaceFromPath("resources/textures/sunshine.jpg", File::JPEG);
  width = sdl->surface_->w;
  height = sdl->surface_->h;
  image_size = sdl->surface_->format->BytesPerPixel * width * height; 
  CreateBuffer(image_size,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |         
               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,     
               staging_buffer, staging_buffer_memory);
  
  vkMapMemory(logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, sdl->surface_->pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, 
    VK_IMAGE_TILING_OPTIMAL,         
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
    texture_image_,
    texture_image_memory_);

  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(staging_buffer, texture_image_, width, height);

  TransitionImageLayout(
      texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(logical_device_, staging_buffer, nullptr);
  vkFreeMemory(logical_device_, staging_buffer_memory, nullptr);
}

void gbengine::Vulkan::CreateTextureSampler() {
  VkPhysicalDeviceProperties properties{};
  VkSamplerCreateInfo sampler_info{};
  VkResult result;
  vkGetPhysicalDeviceProperties(physical_device_, &properties);
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_FALSE; 
  sampler_info.maxAnisotropy = 1.0f;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  result = vkCreateSampler(logical_device_, &sampler_info, nullptr,
                           &texture_sampler_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create texture sampler! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to create texture sampler!");
  }
}



void gbengine::Vulkan::CreateImage(uint32_t width, uint32_t height,
                                   VkFormat format, VkImageTiling tiling,
                                   VkImageUsageFlags usage,
                                   VkMemoryPropertyFlags properties,
                                   VkImage& image,
                                   VkDeviceMemory& image_memory) {
  VkResult result;
  VkImageCreateInfo image_info{};
  VkMemoryRequirements memory_requirements;
  VkMemoryAllocateInfo allocate_info{};

  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.flags = 0;

  result = vkCreateImage(logical_device_, &image_info, nullptr, &image);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create image! {}", VkResultToString(result));
    throw std::runtime_error("Failed to create image! " +
                             VkResultToString(result));
  }

  vkGetImageMemoryRequirements(logical_device_, image,
                               &memory_requirements);

  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  allocate_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits, properties);

  result =
      vkAllocateMemory(logical_device_, &allocate_info, nullptr, &image_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate image memory! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to allocate image memory! " +
                             VkResultToString(result));
  }
  vkBindImageMemory(logical_device_, image, image_memory, 0);
}


void gbengine::Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                    VkMemoryPropertyFlags properties,
                                    VkBuffer& buffer,
                                    VkDeviceMemory& buffer_memory) { 
  VkBufferCreateInfo buffer_info{};
  VkMemoryRequirements memory_requirements;
  VkMemoryAllocateInfo allocate_info{};
  VkResult result; 
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  result = vkCreateBuffer(logical_device_, &buffer_info, nullptr, &buffer); 
  if (result != VK_SUCCESS) { 
    spdlog::critical("Failed to create buffer! {}", VkResultToString(result)); 
    throw std::runtime_error("Failed to create buffer" +
                             VkResultToString(result));
  }
  vkGetBufferMemoryRequirements(logical_device_, buffer,
                                &memory_requirements);

  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; 
  allocate_info.allocationSize = memory_requirements.size;      
  allocate_info.memoryTypeIndex =                               
      FindMemoryType(memory_requirements.memoryTypeBits,        
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |      
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); 
  result = vkAllocateMemory(logical_device_, &allocate_info, nullptr,
                            &buffer_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate buffer memory {} ",
      VkResultToString(result)); 
    throw std::runtime_error("Failed to allocate buffer memory " +
                             VkResultToString(result)); 
  }
  vkBindBufferMemory(logical_device_, buffer, buffer_memory, 0);
}

void gbengine::Vulkan::CopyBuffer(VkBuffer source_buffer,
                                  VkBuffer destination_buffer,
                                  VkDeviceSize size) {
  VkCommandBufferAllocateInfo allocate_info{};
  VkCommandBuffer command_buffer; 
  VkCommandBufferBeginInfo begin_info{}; 
  VkBufferCopy copy_region{};
  VkSubmitInfo submit_info{};

  command_buffer = BeginSingleTimeCommands(command_pool_, logical_device_);

  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1,
                  &copy_region);
  EndSingleTimeCommands(command_buffer, command_pool_, logical_device_, 
                        graphics_queue_); 
}

uint32_t gbengine::Vulkan::FindMemoryType(uint32_t type_filter,
                                      VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties);
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) &&
       (memory_properties.memoryTypes[i].propertyFlags & properties) ==
        properties) {
      return i;
    }
  }
  spdlog::critical("failed to find suitable memory type!");
  throw std::runtime_error("failed to find suitable memory type!");
}

void gbengine::Vulkan::CreateTextureImageView() {
  texture_image_view_ = CreateImageView(texture_image_, VK_FORMAT_R8G8B8A8_SRGB,
                                        VK_IMAGE_ASPECT_COLOR_BIT);
}

VkImageView gbengine::Vulkan::CreateImageView(VkImage image, VkFormat format,
                                              VkImageAspectFlags aspect_flag) {
  VkImageViewCreateInfo view_info{};
  VkImageView image_view; 
  VkResult result;
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image; 
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format; 
  view_info.subresourceRange.aspectMask = aspect_flag;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  result = vkCreateImageView(logical_device_, &view_info, nullptr,
                             &image_view);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create texture image view {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create texture image view " +
                             VkResultToString(result));
  }
  return image_view; 
}

void gbengine::Vulkan::TransitionImageLayout(VkImage image, VkFormat format,
                                             VkImageLayout old_layout,
                                             VkImageLayout new_layout) {
  VkImageMemoryBarrier barrier{};
  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;
  VkCommandBuffer command_buffer =
      BeginSingleTimeCommands(command_pool_, logical_device_);

  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    spdlog::warn("Unsupported Layout translation!");
    throw std::invalid_argument("Unsupported Layout translation!");
  }
  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0,
                       nullptr, 0,
                       nullptr, 1,
                       &barrier);
  EndSingleTimeCommands(command_buffer, command_pool_, logical_device_,
                        graphics_queue_);
}
VkCommandBuffer gbengine::BeginSingleTimeCommands(VkCommandPool command_pool, 
                                                  VkDevice logical_device) {  
  VkCommandBufferAllocateInfo allocate_info{};
  VkCommandBufferBeginInfo begin_info{}; 
  VkCommandBuffer command_buffer; 
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
  allocate_info.commandPool = command_pool;  
  allocate_info.commandBufferCount = 1; 
    
  vkAllocateCommandBuffers(logical_device, &allocate_info, &command_buffer);  
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; 
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; 
  vkBeginCommandBuffer(command_buffer, &begin_info); 
  return command_buffer;
}

void gbengine::EndSingleTimeCommands(VkCommandBuffer command_buffer, 
                                     VkCommandPool command_pool, 
                                     VkDevice logical_device, VkQueue queue) {
  VkSubmitInfo submit_info{}; 
  vkEndCommandBuffer(command_buffer); 
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 
  submit_info.commandBufferCount = 1; 
  submit_info.pCommandBuffers = &command_buffer;

  vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue); 
  vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
} 

void gbengine::Vulkan::CopyBufferToImage(VkBuffer buffer, VkImage image,
                                         uint32_t width, uint32_t height) {
  VkCommandBuffer command_buffer =
      BeginSingleTimeCommands(command_pool_, logical_device_);
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};
  vkCmdCopyBufferToImage(command_buffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &region);
  EndSingleTimeCommands(command_buffer, command_pool_, logical_device_, 
                        graphics_queue_); 
}
void gbengine::Vulkan::CreateSyncObjects() {
  semaphore_.image_available_.resize(kMaxFramesInFlight);
  semaphore_.render_finished_.resize(kMaxFramesInFlight);
  in_flight_fence_.resize(kMaxFramesInFlight);
  VkSemaphoreCreateInfo semaphore_info{};
  VkFenceCreateInfo fence_info{};
  std::array<VkResult,3> result;

  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;


  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    result[0] = vkCreateSemaphore(logical_device_, &semaphore_info, nullptr,
                                  &semaphore_.image_available_[i]);
    result[1] = vkCreateSemaphore(logical_device_, &semaphore_info, nullptr,
                                  &semaphore_.render_finished_[i]);
    result[2] = vkCreateFence(logical_device_, &fence_info, nullptr,
                              &in_flight_fence_[i]);
  }
}

void gbengine::Vulkan::DrawFrame(SDL_Window* window_, SDL_Event *event) {
  vkWaitForFences(logical_device_, 1, &in_flight_fence_[current_frame_],
                  VK_TRUE, UINT64_MAX);
  uint32_t image_index = 0;
  VkResult result =
      vkAcquireNextImageKHR(logical_device_, swap_chain_.KHR_, UINT64_MAX,
                            semaphore_.image_available_[current_frame_],
                            VK_NULL_HANDLE, &image_index);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapChain(window_, event);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    spdlog::critical("Failed to acquire swap chain image! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to acquire swap chain image!");
  }
  
  UpdateUniformBuffer(current_frame_);
  vkResetFences(logical_device_, 1, &in_flight_fence_[current_frame_]);
  vkResetCommandBuffer(command_buffers_[current_frame_], 0);
  RecordCommandBuffer(command_buffers_[current_frame_], image_index);

  VkSubmitInfo submit_info{};
  VkSemaphore wait_semaphores[] = {semaphore_.image_available_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signal_semaphores[] = {
      semaphore_.render_finished_[current_frame_]};
  VkPresentInfoKHR present_info{};
  VkSwapchainKHR swap_chains[] = { swap_chain_.KHR_ };

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1; 
  submit_info.signalSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[current_frame_];
  submit_info.pSignalSemaphores = signal_semaphores;

  result = vkQueueSubmit(graphics_queue_, 1, &submit_info,
                         in_flight_fence_[current_frame_]);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to submit draw command buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to submit draw command buffer!" +
                             VkResultToString(result));
  }

  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores; 
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;

  result = vkQueuePresentKHR(present_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      frame_buffer_resized_) {
    frame_buffer_resized_ = false;
    RecreateSwapChain(window_, event);
  } else if (result != VK_SUCCESS) {
    spdlog::critical("Failed to present queue! {}", VkResultToString(result));
    throw std::runtime_error("Failed to present queue!" +
                             VkResultToString(result));
  }
  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;
}

void gbengine::Vulkan::UpdateUniformBuffer(uint32_t current_image) {
  static auto start_time = std::chrono::high_resolution_clock::now();
  auto current_time = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
               current_time - start_time).count();
  UniformBufferObject ubo{};

  ubo.mode1 = glm::rotate(glm::mat4(1.0f), time * glm::radians(180.0f), 
                          glm::vec3(0.0f, 0.0f, 1.0f)); 

  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), 
                         glm::vec3(0.0f, 0.0f, 0.0f),
                         glm::vec3(0.0f, 0.0f, 1.0f));

  ubo.proj = glm::perspective(
      glm::radians(45.0f), 
      swap_chain_.extent_.width / (float)swap_chain_.extent_.height, 0.1f,
      10.0f);

  ubo.proj[1][1] *= -1;

  memcpy(buffer_.uniform_mapped_[current_image], &ubo, sizeof(ubo));
}

void gbengine::Vulkan::CreateSurface(SDL_Window* window_) {
  if (SDL_Vulkan_CreateSurface(window_, instance_, &surface_)) {
    return;
  }
}

gbengine::Vulkan::Vulkan(SDL* sdl, Application app) {
  InitVulkan(sdl, app);
}

gbengine::Vulkan::~Vulkan() {
  VkResult result;
  CleanUpSwapChain();

  vkDestroySampler(logical_device_, texture_sampler_, nullptr);
  vkDestroyImageView(logical_device_, texture_image_view_, nullptr);
  vkDestroyImage(logical_device_, texture_image_, nullptr);
  vkFreeMemory(logical_device_, texture_image_memory_, nullptr);

  vkDestroyDescriptorPool(logical_device_, descriptor_pool_, nullptr);
  descriptor_pool_ = VK_NULL_HANDLE;
  vkDestroyDescriptorSetLayout(logical_device_, descriptor_set_layout_,
                               nullptr);
  descriptor_set_layout_ = VK_NULL_HANDLE;
  for (size_t i = 0; i < kMaxFramesInFlight; i++) { 
    vkDestroyBuffer(logical_device_, buffer_.uniform_[i], nullptr); 
    buffer_.uniform_[i] = VK_NULL_HANDLE;
    vkFreeMemory(logical_device_, buffer_.uniform_memory_[i], nullptr);
    buffer_.uniform_memory_[i] = VK_NULL_HANDLE;
  }

  vkDestroyDescriptorSetLayout(logical_device_, descriptor_set_layout_,
                               nullptr);

  descriptor_set_layout_ = VK_NULL_HANDLE;
  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    vkDestroySemaphore(logical_device_, semaphore_.image_available_[i],
                       nullptr);
    semaphore_.image_available_[i] = VK_NULL_HANDLE;
    vkDestroySemaphore(logical_device_, semaphore_.render_finished_[i],
                       nullptr);
    semaphore_.render_finished_[i] = VK_NULL_HANDLE;
    vkDestroyFence(logical_device_, in_flight_fence_[i], nullptr);
    in_flight_fence_[i] = VK_NULL_HANDLE;
  }

  vkDestroyCommandPool(logical_device_, command_pool_, nullptr);
  command_pool_ = VK_NULL_HANDLE;
  // Destroy vertex buffer
  if (buffer_.vertex_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(logical_device_, buffer_.vertex_, nullptr);
    buffer_.vertex_ = VK_NULL_HANDLE;
  }
  if (buffer_.vertex_memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(logical_device_, buffer_.vertex_memory_, nullptr);
    buffer_.vertex_memory_ = VK_NULL_HANDLE;
  }

  // Destroy index buffer
  if (buffer_.index_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(logical_device_, buffer_.index_, nullptr);
    buffer_.index_ = VK_NULL_HANDLE; 
  }
  if (buffer_.index_memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(logical_device_, buffer_.index_memory_, nullptr);
    buffer_.index_memory_ = VK_NULL_HANDLE;
  }


  // FIXME!: Your suppose to destory the framebuffer once the vulkan
  // instance goes out of scope, However when we do this here, the
  // frame buffer somehow doesn't exist. Find out where the frame buffer
  // is deallocating itself.
  //                         
  //for (VkFramebuffer frame_buffer : swap_chain_.frame_buffer_) {  
  //  
  // // Tried fixing it by checking if it's null, doesn't work.
  //  if (frame_buffer == VK_NULL_HANDLE) {
  //    continue;
  //  }
  //  vkDestroyFramebuffer(logical_device_, frame_buffer, nullptr);
  //}

  vkDestroyPipeline(logical_device_, graphics_pipeline_, nullptr);
  graphics_pipeline_ = VK_NULL_HANDLE;

  vkDestroyPipelineLayout(logical_device_, pipeline_layout_, nullptr);
  pipeline_layout_ = VK_NULL_HANDLE;

  vkDestroyRenderPass(logical_device_, render_pass_, nullptr);
  render_pass_ = VK_NULL_HANDLE;

  vkDestroyDevice(logical_device_, nullptr);
  logical_device_ = VK_NULL_HANDLE;

  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  surface_ = VK_NULL_HANDLE;

  if (ValidationLayersEnabled && debug_messenger_ != VK_NULL_HANDLE) {
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);
  instance_ = VK_NULL_HANDLE;
  return;
}

std::vector<char> gbengine::ReadFile(const std::string& filename) {
  size_t file_size;
  std::vector<char> buffer;
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    spdlog::critical("failed to open file: {}", filename);
    throw std::runtime_error("");
  }
  file_size = static_cast<size_t>(file.tellg());
  buffer.resize(file_size);
  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();
  return buffer;
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