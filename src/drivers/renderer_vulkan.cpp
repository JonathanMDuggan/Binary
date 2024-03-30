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
void gbengine::Vulkan::InitVulkanInstance(SDL_Window* window, Application app) {
  VkInstanceCreateInfo instance_info{};
  VkApplicationInfo app_info{};
  VkResult result;
  if (ValidationLayersEnabled && !VulkanValidationLayerSupported()) {
    spdlog::critical("Validation layers requested, but not available!");
  }

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

  std::vector<const char*> sdl_extensions = GetExtensions(window);
  instance_info.enabledExtensionCount =
      static_cast<uint32_t>(sdl_extensions.size());
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

  result = vkCreateInstance(&instance_info, nullptr, &instance_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Vulkan instance");
    std::runtime_error("Failed to create Vulkan instance");
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

bool gbengine::Vulkan::IsDeviceSuitable(VkPhysicalDevice phyiscal_device) {
  QueueFamilyIndices indices = FindQueueFamilies(phyiscal_device);
  bool extension_supported = CheckDeviceExtensionSupport(phyiscal_device);
  bool swap_chain_adequate = false;
  if (extension_supported) {
    SwapChainSupportDetails swap_chain_support =
        QuerySwapChainSupport(phyiscal_device);
    swap_chain_adequate = !swap_chain_support.formats.empty() &&
                          !swap_chain_support.present_modes.empty();
  }
  return indices.IsComplete() && extension_supported && swap_chain_adequate;
}

bool gbengine::Vulkan::CheckDeviceExtensionSupport(
    VkPhysicalDevice device_) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device_, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device_, nullptr, &extension_count,
                                       available_extensions.data());

  std::set<std::string> required_extensions(device_extensions.begin(),
                                            device_extensions.end());
  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
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

void gbengine::Vulkan::PickPhysicalDevice() {
  uint32_t device_count = 0;

  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0) {
    spdlog::critical("Failed to find GPU with vulkan support!");
    throw std::runtime_error("");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (const auto& device_ : devices) {
    if (IsDeviceSuitable(device_)) {
      physical_device_ = device_;
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    spdlog::critical("Failed to find a suitable GPU!");
    throw std::runtime_error("");
  }
}

void gbengine::Vulkan::CreateLogicalDevice() {
  float queue_priority = 1.0f;
  VkDeviceCreateInfo device_info;
  VkPhysicalDeviceFeatures device_features{};
  QueueFamilyIndices indinces = FindQueueFamilies(physical_device_);
  VkDeviceQueueCreateInfo queue_info;
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

  device_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size()),
      .pQueueCreateInfos = queue_create_info.data(),
      .pEnabledFeatures = &device_features};

  device_info.enabledExtensionCount =
      static_cast<uint32_t>(device_extensions.size());
  device_info.ppEnabledExtensionNames = device_extensions.data();

  if (ValidationLayersEnabled) {
    device_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    device_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    device_info.enabledLayerCount = 0;
  }
  VkResult result =
      vkCreateDevice(physical_device_, &device_info, nullptr, &device_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create logical device");
  } else {
    spdlog::info("vkCreateDevice returned with the following: {}",
                 VkResultToString(result));
  }

  vkGetDeviceQueue(device_, indinces.graphics_family.value(), 0, &graphics_queue_);
  vkGetDeviceQueue(device_, indinces.present_family.value(), 0, &present_queue_);
}

// Vulkan Queue

gbengine::QueueFamilyIndices gbengine::Vulkan::FindQueueFamilies(
    VkPhysicalDevice phyiscal_device) {
  VkBool32 present_support;
  QueueFamilyIndices indices;
  int i = 0;
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(phyiscal_device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(phyiscal_device, &queue_family_count,
                                           queue_families.data());
  for (const auto& queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(phyiscal_device, i, surface_,
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

std::vector<const char*> gbengine::Vulkan::GetExtensions(
    SDL_Window* window) {
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window, &sdl_extension_count,
                                        nullptr)) {
    spdlog::critical(
        "Failed to get the number of Vulkan instance extensions from SDL");
    return {};
  }

  std::vector<const char*> extensions(sdl_extension_count);
  if (sdl_extension_count > 0) {
    if (!SDL_Vulkan_GetInstanceExtensions(window, &sdl_extension_count,
                                          extensions.data())) {
      spdlog::critical("Failed to get the Vulkan instance extensions from SDL");
      return {};
    }
  }

  if (ValidationLayersEnabled) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void gbengine::Vulkan::InitVulkanValidationLayers() {}

// Vulkan SwapChain stuff

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

VkSurfaceFormatKHR gbengine::Vulkan::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available_format : available_formats) {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
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
    SDL_Window* window,
    const VkSurfaceCapabilitiesKHR& capabilities) {
  int width, height = 0;

  SDL_Vulkan_GetDrawableSize(window, &width, &height);

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

void gbengine::Vulkan::CreateSwapChain(SDL_Window* window) {
  uint32_t image_count = 0;
  SwapChainSupportDetails swap_chain_support =
      QuerySwapChainSupport(physical_device_);
  VkSurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode =
      ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent = ChooseSwapExtent(window, swap_chain_support.capabilities);
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

  VkResult result = vkCreateSwapchainKHR(device_, &swap_chain_info, nullptr,
                                         &swap_chain_.KHR_);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create swap chain! {} on line {} in file {}",
                     VkResultToString(result), __LINE__, __FILE__);
  }

  vkGetSwapchainImagesKHR(device_, swap_chain_.KHR_, &image_count, nullptr);
  swap_chain_.images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swap_chain_.KHR_, &image_count,
                          swap_chain_.images_.data());

  swap_chain_.image_format_ = surface_format.format;
  swap_chain_.extent_ = extent;
}

void gbengine::Vulkan::RecreateSwapChain(SDL_Window* window, SDL_Event* event) {
  int width = 0;
  int height = 0;
  SDL_Vulkan_GetDrawableSize(window, &width, &height);
  while (width == 0 || height == 0) {
    SDL_Vulkan_GetDrawableSize(window, &width, &height);
    SDL_WaitEvent(event);
  }
  vkDeviceWaitIdle(device_);
  CleanUpSwapChain();
  CreateSwapChain(window);
  CreateImageViews();
  CreateFrameBuffer();
}

void gbengine::Vulkan::CleanUpSwapChain() {
  for (uint32_t i = 0; i < swap_chain_.frame_buffer_.size(); i++) {
    vkDestroyFramebuffer(device_, swap_chain_.frame_buffer_[i], nullptr);
  }
  for (uint32_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    vkDestroyImageView(device_, swap_chain_.image_views_[i], nullptr);
  }
  vkDestroySwapchainKHR(device_, swap_chain_.KHR_, nullptr);
}
// Vulkan Image Views

void gbengine::Vulkan::CreateImageViews() {
  VkImageViewCreateInfo image_view_info{};
  swap_chain_.image_views_.resize(swap_chain_.images_.size());
  for (size_t i = 0; i < swap_chain_.images_.size(); i++) {
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.image = swap_chain_.images_[i];
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = swap_chain_.image_format_;
    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = 1;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(device_, &image_view_info, nullptr,
                                        &swap_chain_.image_views_[i]);
    if (result != VK_SUCCESS) {
      spdlog::error("Failed to create image view! {} on line {} in file {}",
                    VkResultToString(result), __LINE__, __FILE__);
    }
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
  VkResult result = vkCreateShaderModule(device_, &shader_module_info, nullptr,
                                         &shader_module);
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
  auto vert_shader_code = ReadFile("shaders/vert.spv");
  auto frag_shader_code = ReadFile("shaders/frag.spv");
  VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);

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
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.pVertexBindingDescriptions = nullptr;
  vertex_input_info.vertexAttributeDescriptionCount = 0;
  vertex_input_info.pVertexAttributeDescriptions = nullptr;

  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_.extent_.width);
  viewport.height = static_cast<float>(swap_chain_.extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  scissor.offset = {0, 0};
  scissor.extent = swap_chain_.extent_;

  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount =
      static_cast<uint32_t>(dynamic_states.size());
  dynamic_state.pDynamicStates = dynamic_states.data();

  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

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
  pipeline_layout_info.setLayoutCount = 0;
  pipeline_layout_info.pSetLayouts = nullptr;
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;
  VkResult result = vkCreatePipelineLayout(device_, &pipeline_layout_info,
                                           nullptr, &pipeline_layout_);
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
  pipeline_info.pDepthStencilState = nullptr;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_state;
  pipeline_info.layout = pipeline_layout_;
  pipeline_info.renderPass = render_pass_;
  pipeline_info.subpass = 0;

  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = -1;
  result = vkCreateGraphicsPipelines(
      device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create graphics pipeline {}",
                     VkResultToString(result));
    throw std::runtime_error("");
  }
  vkDestroyShaderModule(device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(device_, frag_shader_module, nullptr);
}

// Vulkan Render Pass

void gbengine::Vulkan::CreateRenderPass() {
  VkAttachmentDescription color_attachment{};
  VkAttachmentReference color_attachment_reference{};
  VkSubpassDescription subpass{};
  VkRenderPassCreateInfo render_pass_info{};
  VkSubpassDependency dependency{};                          

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

  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;                                            
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;   
  dependency.srcAccessMask = 0;                                            
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;   
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;        

  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;       
  render_pass_info.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass(device_, &render_pass_info, nullptr,
                                       &render_pass_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Render Pass!: {}",
                     VkResultToString(result));
    throw std::runtime_error("");
  }
}

// Vulkan Frame Buffer

void gbengine::Vulkan::CreateFrameBuffer() {
  VkResult result;
  swap_chain_.frame_buffer_.resize(swap_chain_.image_views_.size());
  for (size_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    VkImageView attachments[] = {swap_chain_.image_views_[i]};
    VkFramebufferCreateInfo frame_buffer_info{};
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_info.renderPass = render_pass_;
    frame_buffer_info.attachmentCount = 1;
    frame_buffer_info.pAttachments = attachments;
    frame_buffer_info.width = swap_chain_.extent_.width;
    frame_buffer_info.height = swap_chain_.extent_.height;
    frame_buffer_info.layers = 1;

    result = vkCreateFramebuffer(
        device_, &frame_buffer_info, nullptr, &swap_chain_.frame_buffer_[i]);
    if (result != VK_SUCCESS) {
      spdlog::critical("Failed to create Frame Buffer!: {}",
                       VkResultToString(result));
      throw std::runtime_error("");
    }
  }
}

// Vulkan Command Stuff

void gbengine::Vulkan::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices =
      FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
  VkResult result =
      vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_);
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
  result = vkAllocateCommandBuffers(device_, &allocate_info,
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
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_color;

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_);
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_.extent_.width);
  viewport.height = static_cast<float>(swap_chain_.extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  scissor.offset = {0,0};
  scissor.extent = swap_chain_.extent_;
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
  vkCmdDraw(command_buffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(command_buffer);
  result = vkEndCommandBuffer(command_buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to record Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to record command buffer!" +
                             VkResultToString(result));
  }
}

void gbengine::Vulkan::CreateSyncObjects() {
  semaphore_.image_available_.resize(kMaxFramesInFlight);
  semaphore_.render_finished_.resize(kMaxFramesInFlight);
  in_flight_fence_.resize(kMaxFramesInFlight);
  VkSemaphoreCreateInfo semaphore_info{};
  VkFenceCreateInfo fence_info{};
  VkResult result[3];

  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;


  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    result[0] = vkCreateSemaphore(device_, &semaphore_info, nullptr,
                                  &semaphore_.image_available_[i]);
    result[1] = vkCreateSemaphore(device_, &semaphore_info, nullptr,
                                  &semaphore_.render_finished_[i]);
    result[2] =
        vkCreateFence(device_, &fence_info, nullptr, &in_flight_fence_[i]);
  }

}

void gbengine::Vulkan::DrawFrame(SDL_Window* window, SDL_Event *event) {
  vkWaitForFences(device_, 1, &in_flight_fence_[current_frame_], VK_TRUE,
                  UINT64_MAX);
  uint32_t image_index = 0;
  VkResult result =
      vkAcquireNextImageKHR(device_, swap_chain_.KHR_, UINT64_MAX,
                            semaphore_.image_available_[current_frame_],
                            VK_NULL_HANDLE, &image_index);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapChain(window, event);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    spdlog::critical("Failed to acquire swap chain image! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  VkSubmitInfo submit_info{};
  VkSemaphore wait_semaphores[] = {semaphore_.image_available_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signal_semaphores[] = {
      semaphore_.render_finished_[current_frame_]};
  VkPresentInfoKHR present_info{};
  VkSwapchainKHR swap_chains[] = { swap_chain_.KHR_ };


  vkResetFences(device_, 1, &in_flight_fence_[current_frame_]);
  vkResetCommandBuffer(command_buffers_[current_frame_], 0);
  RecordCommandBuffer(command_buffers_[current_frame_], image_index);

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
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

  vkQueuePresentKHR(present_queue_, &present_info);

  if (result != VK_SUBOPTIMAL_KHR || frame_buffer_resized_) {
    frame_buffer_resized_ = false;
    RecreateSwapChain(window, event);
  } else if (result != VK_SUCCESS) {
    spdlog::critical("Failed to present queue! {}", VkResultToString(result));
    throw std::runtime_error("Failed to present queue!" +
                             VkResultToString(result));
  }
  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;
}

void gbengine::Vulkan::InitVulkan(SDL_Window* window, Application app,
                                  SDL_Event *event) {
  if (ValidationLayersEnabled) spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing Vulkan Drivers");
  InitVulkanInstance(window, app);
  SetupDebugMessenger();
  CreateSurface(window);
  PickPhysicalDevice();
  CreateLogicalDevice();
  spdlog::info("Initializing Vulkan Presentation Layer");
  CreateSwapChain(window);
  CreateImageViews();
  spdlog::info("Creating Vulkan Graphics Pipeline");
  CreateRenderPass();
  CreateGraphicsPipeline();
  CreateFrameBuffer();
  CreateCommandPool();
  CreateCommandBuffer();
  CreateSyncObjects();
}

void gbengine::Vulkan::CreateSurface(SDL_Window* window) {
  if (SDL_Vulkan_CreateSurface(window, instance_, &surface_)) {
    return;
  }
}
gbengine::Vulkan::Vulkan(SDL_Window* window, Application app, SDL_Event* event) {
  InitVulkan(window, app, event);
}

gbengine::Vulkan::~Vulkan() {
  CleanUpSwapChain();
  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    vkDestroySemaphore(device_, semaphore_.image_available_[i], nullptr);
    semaphore_.image_available_[i] = VK_NULL_HANDLE;
    vkDestroySemaphore(device_, semaphore_.render_finished_[i], nullptr);
    semaphore_.render_finished_[i] = VK_NULL_HANDLE;
    vkDestroyFence(device_, in_flight_fence_[i], nullptr);
    in_flight_fence_[i] = VK_NULL_HANDLE;
  }

  vkDestroyCommandPool(device_, command_pool_, nullptr);
  command_pool_ = VK_NULL_HANDLE;

  for (auto frame_buffer : swap_chain_.frame_buffer_) {
    vkDestroyFramebuffer(device_, frame_buffer, nullptr);
  }

  vkDestroyPipeline(device_, pipeline_, nullptr);
  pipeline_ = VK_NULL_HANDLE;

  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  pipeline_layout_ = VK_NULL_HANDLE;

  vkDestroyRenderPass(device_, render_pass_, nullptr);
  render_pass_ = VK_NULL_HANDLE;

  vkDestroyDevice(device_, nullptr);
  device_ = VK_NULL_HANDLE;

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