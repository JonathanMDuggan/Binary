#include"include/gb_gui.h"
gbengine::VulkanViewport::VulkanViewport(gbVulkanGraphicsHandler vulkan) :
  logical_device_(vulkan.device),
  graphics_queue_(vulkan.graphics_queue),
  command_pool_(vulkan.command_pool),
  descriptor_pool_(vulkan.descriptor_pool),
  descriptor_set_layout_(vulkan.descriptor_set_layout) {

}

void gbengine::VulkanViewport::Destory() {
  vkDestroySampler(*logical_device_, texture_sampler_, allocator_.get());
  vkDestroyImageView(*logical_device_, texture_image_view_, allocator_.get());
  vkDestroyImage(*logical_device_, texture_image_, allocator_.get());
  vkFreeMemory(*logical_device_, texture_image_memory_, allocator_.get());
}

void gbengine::VulkanViewport::Free() {
  vkFreeDescriptorSets(*logical_device_, *descriptor_pool_, 1,
                       &descriptor_set_);
}

void gbengine::VulkanViewport::Update(void* array_data) {
}

void gbengine::VulkanViewport::LoadFromPath(const char* file_path) {
}

void gbengine::VulkanViewport::LoadFromArray(void* array_data,
                                             VkDeviceSize array_size,
                                             uint32_t w, uint32_t h){
  
}
