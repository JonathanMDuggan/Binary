#include "include/renderer_vulkan.h"

void gbengine::Vulkan::CreateVertexBuffer() {
  VkDeviceSize buffer_size = sizeof(vertices_[0]) * vertices_.size();
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  void* data;

  CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               staging_buffer, staging_buffer_memory);

  vkMapMemory(logical_device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vertices_.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer_memory);

  CreateBuffer(
      buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_.vertex_,
      buffer_.vertex_memory_);

  CopyBuffer(staging_buffer, buffer_.vertex_, buffer_size);
  vkDestroyBuffer(logical_device_, staging_buffer, allocator_);
  vkFreeMemory(logical_device_, staging_buffer_memory, allocator_);
}
