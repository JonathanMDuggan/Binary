// File: gbengine.cpp
// Programmer: Jonathan M Duggan
// Date: 2024-03-14
// Purpose: Gameboy Engine start up file
// Version: 0.0.0
// 
// NOTE: This file is heavliy commented becasue I don't know what im writing
//       I'm following a guide online called https://vkguide.dev/.

#pragma once 
#define VMA_VULKAN_VERSION 1002000
#include "include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

void gbengine::GameBoyEngine::InitSDL() {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  sdl.window = SDL_CreateWindow(app_info.name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, config.resolution.width,
                                config.resolution.height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}
// A Vulkan instance is vulkan itself, while the device is the hardware on your
// computer
void gbengine::GameBoyEngine::InitVulkanInstanceAndDevice() {
  vkb::Device vkb_device;
  vkb::InstanceBuilder builder;
  vkb::Instance vkb_inst;
  vkb::PhysicalDevice physical_device;
  std::vector<const char*> kExtensionNames;
  std::vector<const char*> kLayerNames{};


  auto inst_ret = builder.set_app_name("GameBoy Engine")
    .request_validation_layers(true)
    .require_api_version(1, 1, 0)
    .use_default_debug_messenger()
    .build();

  // The first thing we do when init vulkan is creating a instance 
  vkb_inst               = inst_ret.value();
  vulkan.inst            = vkb_inst.instance;
  vulkan.debug_messenger = vkb_inst.debug_messenger;

  SDL_bool error = SDL_Vulkan_CreateSurface(sdl.window, 
                                            vulkan.inst, 
                                            &vulkan.surface);
  if (error) {
    std::cout << 
      "[gbEngine] SDL and Vulkan could not communicate to create a surface\n";
  }

  // Vulkan needs a physical device to interface with (your graphics card)
  // the code below does some magic where it finds the best physical device
  // for the renederer
  vkb::PhysicalDeviceSelector selector{vkb_inst};
  physical_device = selector
    .set_minimum_version(1, 1)
    .set_surface(vulkan.surface)
    .select()
    .value();

  vkb::DeviceBuilder device_builder{physical_device};
  vkb_device    = device_builder.build().value();
  vulkan.device = vkb_device.device;
  vulkan.gpu    = physical_device.physical_device;
   
  // Vulkan gets the list of graphic hardware on your computer, and decides
  // what hardware is appropriate for the application.
  vulkan.queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
  vulkan.graphics_queue_family = vkb_device
    .get_queue_index(vkb::QueueType::graphics)
    .value();
}

// Vulkan Swapchain allows the application to keep the images rendered on the
// screen
void gbengine::GameBoyEngine::InitVulkanSwapChain() {
  vkb::SwapchainBuilder swapchain_builder{vulkan.gpu, vulkan.device,
                                          vulkan.surface};

  vkb::Swapchain vkb_swapchain = swapchain_builder
    .use_default_format_selection()
    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
    .set_desired_extent(config.resolution.width, config.resolution.height)
    .build()
    .value();

  vulkan.swapchain              = vkb_swapchain.swapchain;
  vulkan.swapchain_images       = vkb_swapchain.get_images().value();
  vulkan.swapchain_image_views  = vkb_swapchain.get_image_views().value();
  vulkan.swapchain_image_format = vkb_swapchain.image_format;
}


void gbengine::GameBoyEngine::InitVulkanCommands() {
  VkCommandPoolCreateInfo command_pool_info = {};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.pNext = nullptr;
  command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_info.queueFamilyIndex = vulkan.graphics_queue_family;

  for (size_t i = 0; i < kFrameOverLap; i++) {
    VK_CHECK(vkCreateCommandPool(vulkan.device, &command_pool_info, nullptr,
                                 &vulkan.frame_data[i].command_pool));
    VkCommandBufferAllocateInfo cmd_alloc_info{};
    cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_alloc_info.pNext = nullptr;
    cmd_alloc_info.commandPool = vulkan.frame_data[i].command_pool;
    cmd_alloc_info.commandBufferCount = 1;
    cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_CHECK(vkAllocateCommandBuffers(vulkan.device, &cmd_alloc_info,
                                  &vulkan.frame_data[i].main_command_buffer));
  }
}

void gbengine::GameBoyEngine::InitVulkanSyncStructures() {
  VkFenceCreateInfo fence_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  VkSemaphoreCreateInfo semaphore_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
  };

  for (int i = 0; i < kFrameOverLap; i++) {
    VK_CHECK(vkCreateFence(vulkan.device, &fence_info, nullptr,
                           &vulkan.frame_data[i].render_fence));

    VK_CHECK(vkCreateSemaphore(vulkan.device, &semaphore_info, nullptr,
                               &vulkan.frame_data[i].swapchain_semaphore));

    VK_CHECK(vkCreateSemaphore(vulkan.device, &semaphore_info, nullptr,
                               &vulkan.frame_data[i].render_semaphore));
  }
}

// Deallocates vulkan from memory when not need, called when application closes
void gbengine::GameBoyEngine::DestoryVulkan() {

  vkDeviceWaitIdle(vulkan.device);
  for (size_t i = 0; i < kFrameOverLap; i++) {
    vkDestroyCommandPool(vulkan.device, vulkan.frame_data[i].command_pool,
                         nullptr);
  }

  vkDestroyInstance(vulkan.inst, nullptr);
  vkDestroySwapchainKHR(vulkan.device, vulkan.swapchain, nullptr);

  for (size_t i = 0; i < vulkan.swapchain_image_views.size(); i++) {
    vkDestroyImageView(vulkan.device, vulkan.swapchain_image_views[i], nullptr);
  }

  vkDestroyDevice(vulkan.device, nullptr);
  vkDestroySurfaceKHR(vulkan.inst, vulkan.surface, nullptr);
  vkb::destroy_debug_utils_messenger(vulkan.inst, vulkan.debug_messenger);
  vkDestroyInstance(vulkan.inst, nullptr);
}
void gbengine::GameBoyEngine::DestorySDL() {
  SDL_DestroyWindow(sdl.window); }

void gbengine::GameBoyEngine::VulkanDraw(){

}

gbengine::GameBoyEngine::~GameBoyEngine() {
  DestoryVulkan();
  DestorySDL();
}

 gbengine::GameBoyEngine::GameBoyEngine(uint32_t mode_flags) {
   app_info.name            = GB_ENGINE_NAME;
   config.resolution.height = kStandardDefinitionHeight;
   config.resolution.width  = kStandardDefinitionWidth;

   InitSDL();
   InitVulkanInstanceAndDevice();
   InitVulkanSwapChain();
   InitVulkanCommands();

 }

