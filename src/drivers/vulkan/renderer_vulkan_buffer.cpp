#include "../include/renderer_vulkan.h"
void retro::Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
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
  result = vkCreateBuffer(logical_device_, &buffer_info, allocator_, &buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create buffer! {}", VkResultToString(result));
    throw std::runtime_error("Failed to create buffer" +
                             VkResultToString(result));
  }
  // We tell the graphic device what our buffer needs and it outputs the memory
  // requirements for our buffer
  vkGetBufferMemoryRequirements(logical_device_, buffer, &memory_requirements);

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
  allocate_info.memoryTypeIndex = FindMemoryType(
                     memory_requirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Now we pass the infomation to the vulkan function that allocates memory to
  // the device. However we don't know where the grahpic device stored the data
  result = vkAllocateMemory(logical_device_, &allocate_info, allocator_,
                            &buffer_memory);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to allocate buffer memory {} ",
                     VkResultToString(result));
    throw std::runtime_error("Failed to allocate buffer memory " +
                             VkResultToString(result));
  }
  
  // This is where this function comes along and binds our buffer to the memory
  // location, that way we know where our buffer is in the memory location.
  vkBindBufferMemory(logical_device_, buffer, buffer_memory, 0);
}

void retro::Vulkan::CopyBuffer(VkBuffer source_buffer,
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