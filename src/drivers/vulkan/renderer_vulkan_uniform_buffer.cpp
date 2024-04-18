#include "../include/renderer_vulkan.h"

void gbengine::Vulkan::CreateUniformBuffers() {
  VkDeviceSize buffer_size = sizeof(UniformBufferObject);
  uniform_buffer_.resize(k_MaxFramesInFlight);
  uniform_buffer_memory_.resize(k_MaxFramesInFlight);
  uniform_buffer_mapped_.resize(k_MaxFramesInFlight);
  for (size_t i = 0; i < k_MaxFramesInFlight; i++) {

    CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 uniform_buffer_[i], uniform_buffer_memory_[i]);

    vkMapMemory(logical_device_, uniform_buffer_memory_[i], 0, buffer_size, 0,
                &uniform_buffer_mapped_[i]);
  }
}

void gbengine::Vulkan::UpdateUniformBuffer(uint32_t current_frame) {
  static auto start_time = std::chrono::high_resolution_clock::now();
  auto current_time = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   current_time - start_time)
                   .count();
  UniformBufferObject uniform_buffer_object {};

  uniform_buffer_object.model = glm::rotate(
      glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  uniform_buffer_object.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), 
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  uniform_buffer_object.proj = glm::perspective(
      glm::radians(45.0f),
      swap_chain_.extent_.width / (float)swap_chain_.extent_.height, 0.1f,
      10.0f);

  uniform_buffer_object.proj[1][1] *= 1;

  memcpy(uniform_buffer_mapped_[current_frame], &uniform_buffer_object,
         sizeof(uniform_buffer_object));
}