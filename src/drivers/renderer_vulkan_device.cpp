#include "include/renderer_vulkan.h"

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

  // check if the physical has the required extensions needed for the
  // application to work
  std::set<std::string> required_extensions(device_extensions.begin(),
                                            device_extensions.end());
  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  // returns 1 if the physical device supports all the required extensions
  return required_extensions.empty();
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

  VkResult result = vkCreateDevice(physical_device_, &device_info, allocator_,
                                   &logical_device_);
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
                                         &format_count, details.formats.data());
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