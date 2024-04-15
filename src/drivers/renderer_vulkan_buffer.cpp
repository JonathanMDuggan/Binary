#include "include/renderer_vulkan.h"
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
  result = vkCreateBuffer(logical_device_, &buffer_info, allocator_, &buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create buffer! {}", VkResultToString(result));
    throw std::runtime_error("Failed to create buffer" +
                             VkResultToString(result));
  }
  vkGetBufferMemoryRequirements(logical_device_, buffer, &memory_requirements);

  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  allocate_info.memoryTypeIndex =
      FindMemoryType(memory_requirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  result = vkAllocateMemory(logical_device_, &allocate_info, allocator_,
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