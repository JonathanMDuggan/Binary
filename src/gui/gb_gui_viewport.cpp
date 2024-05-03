#include"include/gb_gui.h"
retro::VulkanViewport::VulkanViewport(gbVulkanGraphicsHandler vulkan,
                                      std::unique_ptr<SDL> sdl): 
  physical_device_(vulkan.physical_device.get()),
  logical_device_(vulkan.logical_device.get()),
  graphics_queue_(vulkan.graphics_queue.get()),
  command_pool_(vulkan.command_pool.get()),
  descriptor_pool_(vulkan.descriptor_pool.get()),
  descriptor_set_layout_(vulkan.descriptor_set_layout.get()),
  sdl_(sdl.get()){
}

void retro::VulkanViewport::Destory() {
  vkDestroySampler(*logical_device_, texture_sampler_, allocator_.get());
  vkDestroyImageView(*logical_device_, texture_image_view_, allocator_.get());
  vkDestroyImage(*logical_device_, texture_image_, allocator_.get());
  vkFreeMemory(*logical_device_, texture_image_memory_, allocator_.get());
}

void retro::VulkanViewport::Free() {
  vkFreeDescriptorSets(*logical_device_, *descriptor_pool_, 1,
                       &descriptor_set_);
}

void retro::VulkanViewport::Update(void* array_data) {

}

void retro::VulkanViewport::LoadFromPath(const char* file_path) {
  ViewPortCreateTextureImage(file_path);
  //ViewPortCreateTextureImageView();
  //ViewPortCreateTextureSampler();
  //ViewPortCreateTextureDescriptorSet();
}

void retro::VulkanViewport::LoadFromArray(void* array_data,
                                             VkDeviceSize array_size,
                                             uint32_t w, uint32_t h){
  //ViewPortLoadImageFromArray(array_data, array_size, w, h);
}

void retro::VulkanViewport::ViewPortCreateTextureImage(const char* image_path) {
  VkDeviceSize image_size = 0; 
  sdl_->InitSurfaceFromPath(image_path, File::PNG);
  image_size = sdl_->surface_->format->BytesPerPixel * sdl_->surface_->w * 
               sdl_->surface_->h;
  LoadImageFromArray(sdl_->surface_->pixels, image_size, sdl_->surface_->w,
                     sdl_->surface_->h);
}

void retro::VulkanViewport::LoadImageFromArray(void* image_data,
                                               VkDeviceSize image_size,
                                               uint32_t w, uint32_t h) {
  void* data;
  VkDeviceMemory staging_buffer_memory;
  VkBuffer staging_buffer;
  CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               staging_buffer, staging_buffer_memory);

  // Data is a pointer to VRAM in your graphics card, and if you
  // don't have a graphics card then it's DDRAM.
  //
  // This whole setup is allocating space in whatever memory channel you're
  // using and puting the image there
  vkMapMemory(*logical_device_, staging_buffer_memory, 0, image_size, 0, &data);

  // We got the pointer to the memory free for us to use, lets put the surface
  // pixels where the data address is pointing at
  memcpy(data, image_data, static_cast<size_t>(image_size));
  vkUnmapMemory(*logical_device_, staging_buffer_memory);

  CreateImage(w, h, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image_,
              texture_image_memory_);

  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_UNORM,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(staging_buffer, texture_image_, w, h);

  TransitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_UNORM,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(*logical_device_, staging_buffer, allocator_.get());
  vkFreeMemory(*logical_device_, staging_buffer_memory, allocator_.get());

  CreateTextureImageView();
}

void retro::VulkanViewport::CreateBuffer(VkDeviceSize size,
                                         VkBufferUsageFlags usage,
                                         VkMemoryPropertyFlags properties,
                                         VkBuffer& buffer,
                                         VkDeviceMemory& buffer_memory) {
  VkBufferCreateInfo buffer_info{};
  VkMemoryRequirements memory_requirements;
  VkMemoryAllocateInfo allocate_info{};
  VkResult result;
  // Graphic devices have specialized areas in its memory for certain buffers,
  // for us to store our buffer. We are defineing our buffer then asking
  // our graphic device, what are the requirements for this buffer.
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  result = vkCreateBuffer(*logical_device_, &buffer_info, allocator_.get(), &buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create buffer! {}", VkResultToString(result));
    throw std::runtime_error("Failed to create buffer" +
                             VkResultToString(result));
  }
  // We tell the graphic device what our buffer needs and it outputs the memory
  // requirements for our buffer
  vkGetBufferMemoryRequirements(*logical_device_, buffer, &memory_requirements);

  // Even though we know what our memory requirements are, we don't know where
  // to store the buffer in the graphic device. This is where we write the
  // allocate info
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  // Graphic devices have memory storage for specialize task, the data is
  // store in a way for the graphics device to process it easily.
  // But we don't know if the graphic device has the specialize type of
  // memory for our buffer, so we pass infomation to this function to find
  // the memory location.
  allocate_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Now we pass the infomation to the vulkan function that allocates memory to
  // the device. However we don't know where the grahpic device stored the data
  result = vkAllocateMemory(*logical_device_, &allocate_info, allocator_.get(),
                            &buffer_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate buffer memory {} ",
                     VkResultToString(result));
    throw std::runtime_error("Failed to allocate buffer memory " +
                             VkResultToString(result));
  }

  // This is where this function comes along and binds our buffer to the memory
  // location, that way we know where our buffer is in the memory location.
  vkBindBufferMemory(*logical_device_, buffer, buffer_memory, 0);
}

void retro::VulkanViewport::CreateImage(uint32_t width, uint32_t height,
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

  result =
      vkCreateImage(*logical_device_, &image_info, allocator_.get(), &image);

  if (result != VK_SUCCESS) { 
    spdlog::critical("Failed to create image! {}", VkResultToString(result)); 
    throw std::runtime_error("Failed to create image! " + 
                             VkResultToString(result));
  }

  vkGetImageMemoryRequirements(*logical_device_, image, &memory_requirements);

  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  allocate_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits, properties);

  result = vkAllocateMemory(*logical_device_, &allocate_info, allocator_.get(),
                            &image_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate image memory! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to allocate image memory! " +
                             VkResultToString(result));
  }
  vkBindImageMemory(*logical_device_, image, image_memory, 0);
}

uint32_t retro::VulkanViewport::FindMemoryType(uint32_t type_filter,
                                           VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(*physical_device_, &memory_properties);
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

void retro::VulkanViewport::TransitionImageLayout(VkImage image,
                                                  VkFormat format,
                                                  VkImageLayout old_layout,
                                                  VkImageLayout new_layout) {
  VkImageMemoryBarrier barrier{};
  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;
  VkCommandBuffer command_buffer =
      BeginSingleTimeCommands(*command_pool_, *logical_device_);

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
                       nullptr, 0, nullptr, 1, &barrier); 
  EndSingleTimeCommands(command_buffer, *command_pool_, *logical_device_,
                        *graphics_queue_);

}

void retro::VulkanViewport::CopyBufferToImage(VkBuffer buffer, VkImage image,
                                              uint32_t width, uint32_t height) {
  VkCommandBuffer command_buffer =
      BeginSingleTimeCommands(*command_pool_, *logical_device_);
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
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  EndSingleTimeCommands(command_buffer, *command_pool_, *logical_device_,
                        *graphics_queue_);
}

void retro::VulkanViewport::CreateTextureImageView() {
//  texture_image_view_ = CreateImageView(
//      texture_image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT); 
}
// how would I get the swapchain??
//void retro::VulkanViewport::CreateImageViews() {
//  swap_chain_.image_views_.resize(swap_chain_.images_.size());
//  for (size_t i = 0; i < swap_chain_.images_.size(); i++) {
//    swap_chain_.image_views_[i] =
//        CreateImageView(swap_chain_.images_[i], swap_chain_.image_format_,
//                        VK_IMAGE_ASPECT_COLOR_BIT);
//  }
//}


