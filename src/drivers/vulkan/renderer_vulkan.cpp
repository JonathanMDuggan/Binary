// File: renderer_vulkan.cpp

#pragma once
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#define VMA_VULKAN_VERSION 1002000
#include "../include/renderer_vulkan.h"
#include "../include/peripherals_sdl.h"

void gbengine::Vulkan::InitVulkan(SDL* sdl, Application app) {
  if (ValidationLayersEnabled) spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing Vulkan Instance");
  InitVulkanInstance(sdl->window_, app);
  spdlog::info("Seting up Vulkan Debug Messenger");
  SetupDebugMessenger();
  spdlog::info("Creating Vulkan surface");
  CreateSurface(sdl->window_);
  spdlog::info("Finding a suitable device that supports Vulkan");
  PickPhysicalDevice();
  spdlog::info("Initializing Vulkan Logical Device");
  CreateLogicalDevice();
  spdlog::info("Initializing Vulkan Presentation Layer");
  CreateSwapChain(sdl->window_);
  CreateImageViews();
  spdlog::info("Creating Vulkan Render Pass");
  CreateRenderPass();
  spdlog::info("Laying out the Vulkan descriptor sets");
  CreateDescriptorSetLayout();
  spdlog::info("Creating Vulkan Graphics Pipeline"); 
  CreatePipelineCache();
  CreateGraphicsPipeline();
  spdlog::info("Creating Vulkan Command Pools ");
  CreateCommandPool();
  //CreateDepthResources();
  CreateFrameBuffer(); 
  spdlog::info("Fetching texture resources to Vulkan");
  CreateTextureImage("resources/textures/main_menu.png");
  CreateTextureImageView();
  CreateTextureSampler();
  CreateVertexBuffer(); 
  CreateIndexBuffer();
  CreateUniformBuffers();
  spdlog::info("Creating Vulkan Descriptor Pools ");
  CreateDescriptorPool();
  spdlog::info("Creating Vulkan Descriptor Sets ");
  CreateDescriptorSets();
  spdlog::info("Creating Vulkan Command Buffers ");
  CreateCommandBuffer();
  spdlog::info("Syncing the Vulkan objects together");
  CreateSyncObjects();
}

void gbengine::Vulkan::DrawFrame() {
  vkWaitForFences(logical_device_, 1, &in_flight_fence_[current_frame_],
                  VK_TRUE, UINT64_MAX);
  uint32_t image_index = 0;
  VkResult result =
      vkAcquireNextImageKHR(logical_device_, swap_chain_.KHR_, UINT64_MAX,
                            semaphore_.image_available_[current_frame_],
                            VK_NULL_HANDLE, &image_index);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapChain(sdl_->window_, &sdl_->event_);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    spdlog::critical("Failed to acquire swap chain image! {}",
                     VkResultToString(result));
    throw std::runtime_error("failed to acquire swap chain image!");
  }
  
  //UpdateUniformBuffer(current_frame_); 
  RecordCommandBuffer(command_buffers_[current_frame_], image_index); 
  vkResetFences(logical_device_, 1, &in_flight_fence_[current_frame_]);
  //vkResetCommandBuffer(command_buffers_[current_frame_], 0);

  VkSubmitInfo submit_info{};
  VkSemaphore wait_semaphores[] = {semaphore_.image_available_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signal_semaphores[] = {
      semaphore_.render_finished_[current_frame_]};
  VkPresentInfoKHR present_info{};
  VkSwapchainKHR swap_chains[] = { swap_chain_.KHR_ };

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1; 
  submit_info.signalSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[current_frame_];
  submit_info.pSignalSemaphores = signal_semaphores;

  result = vkQueueSubmit(graphics_queue_, 1, &submit_info,
                         in_flight_fence_[current_frame_]);

  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to submit draw command buffer! {}",
                     VkResultToString(result));
    throw std::runtime_error("Failed to submit draw command buffer!" +
                             VkResultToString(result));
  }

  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores; 
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;

  result = vkQueuePresentKHR(present_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      frame_buffer_resized_) {
    frame_buffer_resized_ = false;
    RecreateSwapChain(sdl_->window_, &sdl_->event_);
  } else if (result != VK_SUCCESS) {
    spdlog::critical("Failed to present queue! {}", VkResultToString(result));
    throw std::runtime_error("Failed to present queue!" +
                             VkResultToString(result));
  }
  current_frame_ = (current_frame_ + 1) % k_MaxFramesInFlight;
}

void gbengine::Vulkan::CreateSurface(SDL_Window* window_) {
  if (SDL_Vulkan_CreateSurface(window_, instance_, &surface_)) {
    return;
  }
}

gbengine::Vulkan::Vulkan(SDL* sdl, Application app) {
  sdl_ = sdl; // TODO: remove all the functions that take sdl as a parameter
              // we now have a member pointer c++ to SDL.
  InitVulkan(sdl, app);
  InitIMGUI(sdl);
}

gbengine::Vulkan::~Vulkan() {
  // Do not destory any objects until the device is idle
  // (do when vulkan isn't renderering anything)
  vkDeviceWaitIdle(logical_device_);
  VkResult result;
  CleanUpSwapChain();
  ImGui_ImplVulkan_Shutdown(); 
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  vkDestroySampler(logical_device_, texture_sampler_, allocator_);
  vkDestroyImageView(logical_device_, texture_image_view_, allocator_);
  vkDestroyImage(logical_device_, texture_image_, allocator_);
  vkFreeMemory(logical_device_, texture_image_memory_, allocator_);

  vkDestroyDescriptorPool(logical_device_, descriptor_pool_, allocator_);
  descriptor_pool_ = VK_NULL_HANDLE;
  vkDestroyDescriptorSetLayout(logical_device_, descriptor_set_layout_,
                               allocator_);
  descriptor_set_layout_ = VK_NULL_HANDLE;

  for (size_t i = 0; i < k_MaxFramesInFlight; i++) { 
    vkDestroyBuffer(logical_device_, uniform_buffer_[i], allocator_); 
    uniform_buffer_[i] = VK_NULL_HANDLE;
    vkFreeMemory(logical_device_, uniform_buffer_memory_[i], allocator_);
    uniform_buffer_memory_[i] = VK_NULL_HANDLE;
  }

  vkDestroyDescriptorSetLayout(logical_device_, descriptor_set_layout_,
                               allocator_);

  descriptor_set_layout_ = VK_NULL_HANDLE;
  for (size_t i = 0; i < k_MaxFramesInFlight; i++) {
    vkDestroySemaphore(logical_device_, semaphore_.image_available_[i],
                       allocator_);
    semaphore_.image_available_[i] = VK_NULL_HANDLE;
    vkDestroySemaphore(logical_device_, semaphore_.render_finished_[i],
                       allocator_);
    semaphore_.render_finished_[i] = VK_NULL_HANDLE;
    vkDestroyFence(logical_device_, in_flight_fence_[i], allocator_);
    in_flight_fence_[i] = VK_NULL_HANDLE;
  }

  vkDestroyCommandPool(logical_device_, command_pool_, allocator_);
  command_pool_ = VK_NULL_HANDLE;
  // Destroy vertex buffer
  if (buffer_.vertex_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(logical_device_, buffer_.vertex_, allocator_);
    buffer_.vertex_ = VK_NULL_HANDLE;
  }
  if (buffer_.vertex_memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(logical_device_, buffer_.vertex_memory_, allocator_);
    buffer_.vertex_memory_ = VK_NULL_HANDLE;
  }

  // Destroy index buffer
  if (buffer_.index_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(logical_device_, buffer_.index_, allocator_);
    buffer_.index_ = VK_NULL_HANDLE; 
  }
  if (buffer_.index_memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(logical_device_, buffer_.index_memory_, allocator_);
    buffer_.index_memory_ = VK_NULL_HANDLE;
  }


  // FIXME!: Your suppose to destory the framebuffer once the vulkan
  // instance goes out of scope, However when we do this here, the
  // frame buffer somehow doesn't exist. Find out where the frame buffer
  // is deallocating itself.
  //                         
  //for (VkFramebuffer frame_buffer : swap_chain_.frame_buffer_) {  
  //  
  // // Tried fixing it by checking if it's null, doesn't work.
  //  if (frame_buffer == VK_NULL_HANDLE) {
  //    continue;
  //  }
  //  vkDestroyFramebuffer(logical_device_, frame_buffer, allocator_);
  //}
  vkDestroyPipelineCache(logical_device_, pipeline_cache_, allocator_);
  pipeline_cache_ = VK_NULL_HANDLE;

  vkDestroyPipeline(logical_device_, graphics_pipeline_, allocator_);

  graphics_pipeline_ = VK_NULL_HANDLE;

  vkDestroyPipelineLayout(logical_device_, pipeline_layout_, allocator_);
  pipeline_layout_ = VK_NULL_HANDLE;

  vkDestroyRenderPass(logical_device_, render_pass_, allocator_);
  render_pass_ = VK_NULL_HANDLE;

  vkDestroyDevice(logical_device_, allocator_);
  logical_device_ = VK_NULL_HANDLE;

  vkDestroySurfaceKHR(instance_, surface_, allocator_);
  surface_ = VK_NULL_HANDLE;

  if (ValidationLayersEnabled && debug_messenger_ != VK_NULL_HANDLE) {
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, allocator_);
  }

  vkDestroyInstance(instance_, allocator_);
  instance_ = VK_NULL_HANDLE;
  return;
}


