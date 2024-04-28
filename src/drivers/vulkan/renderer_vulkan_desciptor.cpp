#include "../include/renderer_vulkan.h"

void gbengine::Vulkan::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> pool_size;
  VkDescriptorPoolCreateInfo pool_info{};
  VkResult result;

  pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_size[0].descriptorCount = static_cast<uint32_t>(k_MaxFramesInFlight);
  pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_size[1].descriptorCount = static_cast<uint32_t>(k_MaxFramesInFlight);

  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  // ImGui needs space for its textures and stuff
  // That's why were using IM_ARRAYSIZE for the pool size count
  pool_info.poolSizeCount =
      static_cast<uint32_t>(IM_ARRAYSIZE(pool_size.data()));
  pool_info.pPoolSizes = pool_size.data(); 
  pool_info.maxSets = static_cast<uint32_t>(k_MaxFramesInFlight * 2); 

  result = vkCreateDescriptorPool(logical_device_, &pool_info, allocator_,
                                  &descriptor_pool_);
  if (result != VK_SUCCESS) {
    spdlog::critical(
        "Failed to create Desciptor Pool! {} on line {} in file {}",
        VkResultToString(result), __LINE__, __FILE__);
    throw std::runtime_error("Failed to create Desciptor Pool!: " +
                             VkResultToString(result));
  }
}

void gbengine::Vulkan::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutCreateInfo layout_info{};
  VkDescriptorSetLayoutBinding uniform_buffer_object_layout_binding{};
  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
  VkResult result;

  uniform_buffer_object_layout_binding.binding = 0;
  uniform_buffer_object_layout_binding.descriptorType = 
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uniform_buffer_object_layout_binding.descriptorCount = 1;
  uniform_buffer_object_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uniform_buffer_object_layout_binding.pImmutableSamplers = nullptr;

  // Bind the image sampler to the descriptor set layout for transformation
  // during the fragmentation stage of the graphics pipeline
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  sampler_layout_binding.pImmutableSamplers = nullptr;

  bindings = {uniform_buffer_object_layout_binding, sampler_layout_binding};

  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_info.pBindings = bindings.data();
  result = vkCreateDescriptorSetLayout(logical_device_, &layout_info,
                                       allocator_, &descriptor_set_layout_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create descriptor set layout! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to create descriptor set layout!" +
                             VkResultToString(result));
  }
}

void gbengine::Vulkan::CreateDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layout(k_MaxFramesInFlight,
                                            descriptor_set_layout_);
  std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
  VkDescriptorSetAllocateInfo allocate_info{};
  VkResult result;
  allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool = descriptor_pool_;
  allocate_info.descriptorSetCount = static_cast<uint32_t>(k_MaxFramesInFlight);
  allocate_info.pSetLayouts = layout.data();
  descriptor_sets_.resize(k_MaxFramesInFlight);

  result = vkAllocateDescriptorSets(logical_device_, &allocate_info,
                                    descriptor_sets_.data());
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create descriptor set!: {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to create descriptor set! " +
                             VkResultToString(result));
  }

  for (size_t i = 0; i < k_MaxFramesInFlight; i++) {
    VkDescriptorBufferInfo uniform_buffer_info{}; 
    VkDescriptorImageInfo image_info{};

    uniform_buffer_info.buffer = uniform_buffer_[i];
    uniform_buffer_info.offset = 0;
    uniform_buffer_info.range = sizeof(UniformBufferObject);

    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture_image_view_;
    image_info.sampler = texture_sampler_;

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; 
    descriptor_writes[0].dstSet = descriptor_sets_[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; 
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &uniform_buffer_info;
    descriptor_writes[0].pImageInfo = nullptr;
    descriptor_writes[0].pTexelBufferView = nullptr;

    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = descriptor_sets_[i];
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0; 
    descriptor_writes[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(logical_device_,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(), 0,
                           nullptr);
  }
}