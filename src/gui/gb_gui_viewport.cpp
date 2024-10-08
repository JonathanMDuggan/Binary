#include"include/gb_gui.h"
binary::VulkanViewport::VulkanViewport(gbVulkanGraphicsHandler vulkan,
                                      SDL* sdl): 
  physical_device_(vulkan.physical_device),
  logical_device_(vulkan.logical_device),
  graphics_queue_(vulkan.graphics_queue),
  command_pool_(vulkan.command_pool),
  descriptor_pool_(vulkan.descriptor_pool),
  imgui_pool(vulkan.imgui_pool),
  texture_descriptor_set_(VK_NULL_HANDLE),
  sdl_(sdl){
}

void binary::VulkanViewport::Destroy() {
  vkDeviceWaitIdle(*logical_device_);
  vkDestroySampler(*logical_device_, texture_sampler_, allocator_);
  vkDestroyImageView(*logical_device_, texture_image_view_, allocator_);
  vkDestroyImage(*logical_device_, texture_image_, allocator_);
  vkFreeMemory(*logical_device_, texture_image_memory_, allocator_);
  ImGui_ImplVulkan_RemoveTexture(texture_descriptor_set_);

  texture_descriptor_set_ = VK_NULL_HANDLE;
}

void binary::VulkanViewport::Free() {
  vkDeviceWaitIdle(*logical_device_);
  vkDestroySampler(*logical_device_, texture_sampler_, allocator_);
  vkDestroyImageView(*logical_device_, texture_image_view_, allocator_);
  vkDestroyImage(*logical_device_, texture_image_, allocator_);
  vkFreeMemory(*logical_device_, texture_image_memory_, allocator_);
  vkFreeDescriptorSets(*logical_device_, *imgui_pool, 1, 
                       &texture_descriptor_set_);
  texture_descriptor_set_ = VK_NULL_HANDLE;
}

void binary::VulkanViewport::Update(void* array_data) {
  vkDeviceWaitIdle(*logical_device_);
  Free();
  LoadImageFromArray(array_data, array_size_, w_, h_);
  CreateTextureImageView();
  CreateTextureSampler();
  CreateTextureDescriptorSet();
}

void binary::VulkanViewport::LoadFromPath(const char* file_path) {
  ViewPortCreateTextureImage(file_path);
  CreateTextureImageView();
  CreateTextureSampler();
  CreateTextureDescriptorSet();
}

void binary::VulkanViewport::LoadFromArray(void* array_data,
                                             VkDeviceSize array_size,
                                             uint32_t w, uint32_t h){
  LoadImageFromArray(array_data, array_size, w, h);
  CreateTextureImageView();
  CreateTextureSampler();
  CreateTextureDescriptorSet();
}

binary::VulkanViewportInfo binary::VulkanViewport::GetViewportInfo() { 
  if (texture_descriptor_set_ == nullptr) {
    spdlog::critical("No texture has been loaded in vulkan viewport!");
    throw std::runtime_error("No texture has been loaded in vulkan viewport!");
  }
  binary::VulkanViewportInfo viewport_info{};
  viewport_info.mips_levels = &mips_levels_; // TODO: THERE IS NO MIPS_LEVELS!
                                             // THIS IS BAD, FIX THIS!    
  viewport_info.texture_sampler = &texture_sampler_; 
  viewport_info.texture_image_view = &texture_image_view_; 
  viewport_info.texture_image_memory = &texture_image_memory_; 
  viewport_info.texture_image = &texture_image_; 
  viewport_info.array_size = &array_size_;
  viewport_info.texture_descriptor_set = &texture_descriptor_set_;
  viewport_info.h = h_;
  viewport_info.w = w_;
  return viewport_info; 
}

binary::VulkanViewport::~VulkanViewport() { Destroy(); }

void binary::VulkanViewport::ViewPortCreateTextureImage(const char* image_path){
  sdl_->InitSurfaceFromPath(image_path, File::PNG);
  w_ = sdl_->surface_->w;
  h_ = sdl_->surface_->h;
  array_size_ = sdl_->surface_->format->BytesPerPixel * w_ * h_;
  LoadImageFromArray(sdl_->surface_->pixels, array_size_, w_, h_);
}

void binary::VulkanViewport::LoadImageFromArray(void* image_data,
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

  vkDestroyBuffer(*logical_device_, staging_buffer, allocator_);
  vkFreeMemory(*logical_device_, staging_buffer_memory, allocator_);
}

void binary::VulkanViewport::CreateBuffer(VkDeviceSize size,
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
  result = vkCreateBuffer(*logical_device_, &buffer_info, allocator_, &buffer);
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
  result = vkAllocateMemory(*logical_device_, &allocate_info, allocator_,
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

void binary::VulkanViewport::CreateImage(uint32_t width, uint32_t height,
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
      vkCreateImage(*logical_device_, &image_info, allocator_, &image);

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
  
  result = vkAllocateMemory(*logical_device_, &allocate_info, allocator_,
                            &image_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate image memory! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to allocate image memory! " +
                             VkResultToString(result));
  }
  vkBindImageMemory(*logical_device_, image, image_memory, 0);
}

uint32_t binary::VulkanViewport::FindMemoryType(uint32_t type_filter,
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

void binary::VulkanViewport::TransitionImageLayout(VkImage image,
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

void binary::VulkanViewport::CopyBufferToImage(VkBuffer buffer, VkImage image,
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

void binary::VulkanViewport::CreateTextureImageView() {
  texture_image_view_ = CreateImageView(
     texture_image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);  
}

void binary::VulkanViewport::CreateTextureSampler() {
  VkPhysicalDeviceProperties properties{};
  VkSamplerCreateInfo sampler_info{};
  VkResult result;
  vkGetPhysicalDeviceProperties(*physical_device_, &properties);
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_NEAREST;
  sampler_info.minFilter = VK_FILTER_NEAREST;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_FALSE;
  sampler_info.maxAnisotropy = 1.0f;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  result = vkCreateSampler(*logical_device_, &sampler_info, allocator_,
                           &texture_sampler_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create texture sampler! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to create texture sampler!");
  }
}

VkImageView binary::VulkanViewport::CreateImageView(
  VkImage image, VkFormat format, VkImageAspectFlags aspect_flag) {
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

  result = vkCreateImageView(*logical_device_, &view_info, allocator_,
                             &image_view);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create texture image view {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create texture image view " +
                             VkResultToString(result));
  }
  return image_view;
}

void binary::VulkanViewport::CreateTextureDescriptorSet() {
  texture_descriptor_set_ =
      ImGui_ImplVulkan_AddTexture(texture_sampler_, texture_image_view_,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}