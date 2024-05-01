#include "../include/renderer_vulkan.h"

// Vulkan Command Stuff
void retro::Vulkan::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
  VkResult result = vkCreateCommandPool(logical_device_, &pool_info, allocator_,
                                        &command_pool_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Command Pool! {}",
                     VkResultToString(result));
    throw std::runtime_error("");
  }
}

void retro::Vulkan::CreateCommandBuffer() {
  command_buffers_.resize(k_MaxFramesInFlight);
  VkCommandBufferAllocateInfo allocate_info{};
  VkResult result;
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount =
      static_cast<uint32_t>(command_buffers_.size());
  result = vkAllocateCommandBuffers(logical_device_, &allocate_info,
                                    command_buffers_.data());
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create Command Buffer! " +
                             VkResultToString(result));
  }
}

void retro::Vulkan::RecordCommandBuffer(VkCommandBuffer command_buffer,
                                           uint32_t image_index) {
  VkCommandBufferBeginInfo begin_info{};
  VkResult result;
  VkRenderPassBeginInfo render_pass_info{};
  VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0}}};
  VkViewport viewport{};
  VkRect2D scissor{};
  VkDeviceSize offsets[] = {0};
  VkBuffer vertex_buffers[] = {buffer_.vertex_};
  VkClearValue clear_values{};
  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  begin_info.pInheritanceInfo = nullptr;
  result = vkBeginCommandBuffer(command_buffer, &begin_info);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to begin recording Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to begin recording command buffer!" +
                             VkResultToString(result));
  }

  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_;
  render_pass_info.framebuffer = swap_chain_.frame_buffer_[image_index];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swap_chain_.extent_;

  clear_values.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_values;

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline_);
  // View Port
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_.extent_.width);
  viewport.height = static_cast<float>(swap_chain_.extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  // Scissor
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_.extent_;
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
  // Vertex Buffer
  vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
  // Index Buffer
  vkCmdBindIndexBuffer(command_buffer, buffer_.index_, 0, VK_INDEX_TYPE_UINT16);
  // Bind Descriptors Sets
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout_, 0, 1,
                          &descriptor_sets_[current_frame_], 0, nullptr);
  // Draw Indexed
  vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices_.size()), 1, 0,
                   0, 0);
  // Draw ImGui
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

  vkCmdEndRenderPass(command_buffer);
  result = vkEndCommandBuffer(command_buffer);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to record Command Buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to record command buffer!" +
                             VkResultToString(result));
  }
}

VkCommandBuffer retro::BeginSingleTimeCommands(VkCommandPool command_pool,
                                                  VkDevice logical_device) {
  VkCommandBufferAllocateInfo allocate_info{};
  VkCommandBufferBeginInfo begin_info{};
  VkCommandBuffer command_buffer;
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandPool = command_pool;
  allocate_info.commandBufferCount = 1;

  vkAllocateCommandBuffers(logical_device, &allocate_info, &command_buffer);
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);
  return command_buffer;
}

void retro::EndSingleTimeCommands(VkCommandBuffer command_buffer,
                                     VkCommandPool command_pool,
                                     VkDevice logical_device, VkQueue queue) {
  VkSubmitInfo submit_info{};
  vkEndCommandBuffer(command_buffer);
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
}