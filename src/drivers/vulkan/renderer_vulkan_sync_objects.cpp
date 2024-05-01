#include "../include/renderer_vulkan.h"


void retro::Vulkan::CreateSyncObjects() {
  semaphore_.image_available_.resize(k_MaxFramesInFlight);
  semaphore_.render_finished_.resize(k_MaxFramesInFlight);
  in_flight_fence_.resize(k_MaxFramesInFlight);
  VkSemaphoreCreateInfo semaphore_info{};
  VkFenceCreateInfo fence_info{};
  std::array<VkResult, 3> result;

  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < k_MaxFramesInFlight; i++) {
    result[0] = vkCreateSemaphore(logical_device_, &semaphore_info, allocator_,
                                  &semaphore_.image_available_[i]);
    result[1] = vkCreateSemaphore(logical_device_, &semaphore_info, allocator_,
                                  &semaphore_.render_finished_[i]);
    result[2] = vkCreateFence(logical_device_, &fence_info, allocator_,
                              &in_flight_fence_[i]);
  }
}
