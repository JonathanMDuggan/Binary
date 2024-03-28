// File: renderer_vulkan.h
#pragma once
#include <string>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <limits>
#include <fstream>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "vk_mem_alloc.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "spdlog/spdlog.h"
#include "peripherals_sdl.h"
namespace gbengine {
extern std::vector<char> ReadFile(const std::string& filename);
extern std::string VkResultToString(VkResult result);
enum VulkanConst { kFrameOverLap = 2, kMaxFramesInFlight = 2};

typedef struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
  bool IsComplete() {
    return graphics_family.has_value() && present_family.has_value();
  }
} QueueFamilyIndices;

class Vulkan {
public:

  VkDevice device_{};
 Vulkan(SDL_Window* window, Application app, SDL_Event* event);
  ~Vulkan();
 void DrawFrame(SDL_Window* window, SDL_Event* event);

private:
  const std::vector<const char*> validation_layers = {
      "VK_LAYER_KHRONOS_validation",
  };
  const std::vector<const char*> device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef NDEBUG
  const bool ValidationLayersEnabled = false;
#else
  const bool ValidationLayersEnabled = true;
#endif
  enum VulkanConst { kFrameOverLap = 2 };

  typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  } SwapChainSupportDetails;

  typedef struct SwapChain {
    VkSwapchainKHR KHR_{};
    std::vector<VkImage> images_{};
    std::vector<VkImageView> image_views_{};
    std::vector<VkFramebuffer> frame_buffer_;
    VkFormat image_format_{};
    VkExtent2D extent_{};
    SwapChainSupportDetails support_details_ = {};
  }SwapChain;

  typedef struct FrameData {
    VkCommandPool command_pool_{};
    VkCommandBuffer main_command_buffer_{};
    VkSemaphore swapchain_semaphore_{}, render_semaphore_{};
    VkFence render_fence_{};

  }FrameData;

  typedef struct Semaphore {
    std::vector<VkSemaphore> image_available_;
    std::vector<VkSemaphore> render_finished_;
  };

  const char* const* instance_validation_layers_{};
  uint32_t sdl_extenstion_count_{};
  const char** kSDLExtensions_{};
  VkInstance instance_; 
  VkQueueFamilyProperties* queue_props_{};
  VkDebugUtilsMessengerEXT debug_messenger_{};
  VkPhysicalDevice physical_device_{};
  VkSurfaceKHR surface_{};
  VkQueue graphics_queue_{};
  VkQueue present_queue_{};
  VkCommandPool command_pool_{};
  SwapChain swap_chain_{};
  uint32_t graphics_queue_family_{};
  FrameData frame_data_[kFrameOverLap] = {};
  VkPipelineLayout pipeline_layout_;
  VkRenderPass render_pass_;
  VkPipeline pipeline_;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkFence>in_flight_fence_;
  Semaphore semaphore_;
  uint32_t current_frame_ = 0;
  VkFence in_flight_fences_{}; 
  bool frame_buffer_resized_ = false; 

  void InitVulkanApplication();
  void InitVulkanInfo();
  void InitVulkanPhysicalDevice();
  void InitVulkanValidationLayers();
  bool IsDeviceSuitable(VkPhysicalDevice physical_device_);

  void InitVulkanInstance(SDL_Window* window, Application app);
  void SetupDebugMessenger();
  void CreateSurface(SDL_Window* window);
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateSwapChain(SDL_Window* window);
  void CreateImageViews();
  void CreateGraphicsPipeline();
  void CreateRenderPass();
  void CreateFrameBuffer();
  void CreateCommandPool();
  void CreateCommandBuffer();
  void CreateSyncObjects();
  void RecreateSwapChain(SDL_Window* window, SDL_Event* event);
  void CleanUpSwapChain();
  void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  int RateDeviceSuitabillity(VkPhysicalDevice device_);
  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& debug_info);
  bool VulkanValidationLayerSupported();

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance_,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);

  void DestroyDebugUtilsMessengerEXT(VkInstance instance_,
                                     VkDebugUtilsMessengerEXT debugMessenger,
                                     const VkAllocationCallbacks* pAllocator);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device_);

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device_);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device_);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);

  VkShaderModule CreateShaderModule(const std::vector<char>& code);
  VkExtent2D ChooseSwapExtent(SDL_Window* window,
                              const VkSurfaceCapabilitiesKHR& capabilities);
  std::vector<const char*> GetExtensions(SDL_Window* window);
  void InitVulkan(SDL_Window* window, gbengine::Application app,
                  SDL_Event *event);
};

}
