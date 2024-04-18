#include "../include/renderer_vulkan.h"

void gbengine::Vulkan::CleanUpSwapChain() {
  for (uint32_t i = 0; i < swap_chain_.frame_buffer_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_.frame_buffer_[i],
                         allocator_);
  }
  for (uint32_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_.image_views_[i],
                       allocator_);
  }
  vkDestroySwapchainKHR(logical_device_, swap_chain_.KHR_, allocator_);
}


void gbengine::Vulkan::RecreateSwapChain(SDL_Window* window_,
                                         SDL_Event* event) {
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
  CreateFrameBuffer();
}

void gbengine::Vulkan::CreateSwapChain(SDL_Window* window_) {
  uint32_t image_count = 0;
  SwapChainSupportDetails swap_chain_support =
      QuerySwapChainSupport(physical_device_);
  VkSurfaceFormatKHR surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats);
  VkPresentModeKHR present_mode =
      ChooseSwapPresentMode(swap_chain_support.present_modes);
  VkExtent2D extent =
      ChooseSwapExtent(window_, swap_chain_support.capabilities);
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
                                         allocator_, &swap_chain_.KHR_);

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

// Vulkan Frame Buffer

void gbengine::Vulkan::CreateFrameBuffer() {
  VkResult result;
  swap_chain_.frame_buffer_.resize(swap_chain_.image_views_.size());
  for (size_t i = 0; i < swap_chain_.image_views_.size(); i++) {
    VkImageView attachments = swap_chain_.image_views_[i];
    VkFramebufferCreateInfo frame_buffer_info{};
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_info.renderPass = render_pass_;
    frame_buffer_info.attachmentCount = 1;
    frame_buffer_info.pAttachments = &attachments;
    frame_buffer_info.width = swap_chain_.extent_.width;
    frame_buffer_info.height = swap_chain_.extent_.height;
    frame_buffer_info.layers = 1;

    result = vkCreateFramebuffer(logical_device_, &frame_buffer_info,
                                 allocator_, &swap_chain_.frame_buffer_[i]);
    if (result != VK_SUCCESS) {
      spdlog::critical("Failed to create Frame Buffer!: {}",
                       VkResultToString(result));
      throw std::runtime_error("");
    }
  }
}


VkExtent2D gbengine::Vulkan::ChooseSwapExtent(
    SDL_Window* window_, const VkSurfaceCapabilitiesKHR& capabilities) {
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
