#include"include/gb_gui.h"
gbengine::VulkanViewport::VulkanViewport(gbVulkanGraphicsHandler vulkan) {
  // Link the viewport to the vulkan instance 
  logical_device_  = vulkan.device;
  graphics_queue_  = vulkan.graphics_queue;
  command_pool_    = vulkan.command_pool;
  descriptor_pool_ = vulkan.descriptor_pool;
  descriptor_set_layout_ = vulkan.descriptor_set_layout;

}

void gbengine::VulkanViewport::Destory() {
  vkDestroySampler(*logical_device_, texture_sampler_, allocator_);
  vkDestroyImageView(*logical_device_, texture_image_view_, allocator_);
  vkDestroyImage(*logical_device_, texture_image_, allocator_);
  vkFreeMemory(*logical_device_, texture_image_memory_, allocator_);
}
