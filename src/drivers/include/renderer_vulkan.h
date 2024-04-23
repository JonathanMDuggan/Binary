// File: renderer_vulkan.h
#pragma once
#include <array>
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
#include <SDL_image.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include "vk_mem_alloc.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "spdlog/spdlog.h"
#include "peripherals_sdl.h"
#include "renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"
namespace gbengine {
// If I don't make these function inline the command buffer stops
// recording when returning for no reason.
extern inline VkCommandBuffer BeginSingleTimeCommands(
    VkCommandPool command_pool, VkDevice logical_device);
extern inline void EndSingleTimeCommands(VkCommandBuffer command_buffer,
                                         VkCommandPool command_pool,
                                         VkDevice logical_device,
                                         VkQueue queue);
typedef struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct Vertex {
  glm::vec2 position_;
  glm::vec3 color_;
  glm::vec2 texture_coordinates_;
  static VkVertexInputBindingDescription GetBindingDescription() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding_description;
  }

  static std::array<VkVertexInputAttributeDescription, 3>
  GetAttributeDesciptions() {
    std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, position_);

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color_);

    attribute_descriptions[2].binding = 0;
    attribute_descriptions[2].location = 2;
    attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[2].offset = offsetof(Vertex, texture_coordinates_);

    return attribute_descriptions;
  }
};
const std::vector<Vertex> vertices_ = {
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
};

const std::vector<uint16_t> indices_ = {0, 1, 2, 2, 3, 0};
extern std::vector<char> ReadFile(const std::string& file_name);
extern std::string VkResultToString(VkResult result);
enum VulkanConst { kFrameOverLap = 2, k_MaxFramesInFlight = 2 };

typedef struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
  bool IsComplete() {
    return graphics_family.has_value() && present_family.has_value();
  }
} QueueFamilyIndices;

class Vulkan : public Renderer {
 public:
  Vulkan(SDL* sdl, Application app);
  ~Vulkan();
  void DrawFrame();

 private:
  const std::vector<const char*> validation_layers = {
      "VK_LAYER_KHRONOS_validation"};
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
  } SwapChain;

  typedef struct FrameData {
    VkCommandPool command_pool_{};
    VkCommandBuffer main_command_buffer_{};
    VkSemaphore swapchain_semaphore_{}, render_semaphore_{};
    VkFence render_fence_{};
  } FrameData;

  typedef struct Semaphore {
    std::vector<VkSemaphore> image_available_;
    std::vector<VkSemaphore> render_finished_;
  };

  typedef struct Buffer {
    VkBuffer vertex_;
    VkBuffer index_;
    VkDeviceMemory vertex_memory_;
    VkDeviceMemory index_memory_;
  };

  // Command Buffers
  VkCommandPool command_pool_{};
  std::vector<VkCommandBuffer> command_buffers_;

  // Graphic Pipeline
  VkPipelineLayout pipeline_layout_;
  VkPipelineCache pipeline_cache_;
  VkPipeline graphics_pipeline_;
  VkRenderPass render_pass_;
  // Extensions and validation layers
  const char* const* instance_validation_layers_{};
  uint32_t sdl_extenstion_count_{};
  const char** kSDLExtensions_{};
  VkQueueFamilyProperties* queue_props_{};
  VkDebugUtilsMessengerEXT debug_messenger_{};

  VkSurfaceKHR surface_{};

  // Graphic device allocation and pointers
  VkInstance instance_;

  VkPhysicalDevice physical_device_{};
  VkQueue present_queue_{};
  uint32_t graphics_queue_family_{};

  SwapChain swap_chain_{};
  FrameData frame_data_[kFrameOverLap] = {};

  // Sync Objects
  std::vector<VkFence> in_flight_fence_;
  Semaphore semaphore_;
  uint32_t current_frame_ = 0;
  VkFence in_flight_fences_{};

  bool frame_buffer_resized_ = false;
  Buffer buffer_{};

  // Descriptor Sets
  std::vector<VkDescriptorSet> descriptor_sets_;

  // Textures
  VkImage texture_image_;
  VkDeviceMemory texture_image_memory_;
  VkImageView texture_image_view_{};
  VkSampler texture_sampler_{};

  // Uniform Buffers
  std::vector<VkBuffer> uniform_buffer_;
  std::vector<VkDeviceMemory> uniform_buffer_memory_;
  std::vector<void*> uniform_buffer_mapped_;

  // Class pointers
  SDL* sdl_;

  // Important stuff
  VkDescriptorPool descriptor_pool_{};
  VkDevice logical_device_{};
  VkAllocationCallbacks* allocator_ = VK_NULL_HANDLE;
  VkDescriptorSetLayout descriptor_set_layout_{};
  VkQueue graphics_queue_{};

  bool IsPhysicalDeviceSuitable(VkPhysicalDevice physical_device);

  void CreateImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage& image,
                   VkDeviceMemory& image_memory);

  void InitVulkanInstance(SDL_Window* window_, Application app);
  void SetupDebugMessenger();
  void CreateSurface(SDL_Window* window_);
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateSwapChain(SDL_Window* window_);
  void CreateImageViews();
  void CreateGraphicsPipeline();
  void CreateRenderPass();
  void CreateFrameBuffer();
  void CreateCommandPool();
  void CreateCommandBuffer();
  void CreateSyncObjects();
  void RecreateSwapChain(SDL_Window* window_, SDL_Event* event);
  void CleanUpSwapChain();
  void CreateDescriptorPool();
  void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void CreateIndexBuffer();
  void CreateVertexBuffer();
  void CreateDescriptorSetLayout();
  void CreateUniformBuffers();
  void CreatePipelineCache();
  void CreateDescriptorSets();
  void CreateTextureImageView();
  void CreateTextureImage(const char* image_path);
  void CreateTextureSampler();
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer& buffer,
                    VkDeviceMemory& buffer_memory);
  void CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer,
                  VkDeviceSize size);
  uint32_t FindMemoryType(uint32_t type_filter,
                          VkMemoryPropertyFlags properties);
  int RateDeviceSuitabillity(VkPhysicalDevice physical_device);
  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& debug_info);
  bool VulkanValidationLayerSupported();
  VkImageView CreateImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspect_flag);
  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance_,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);
  void DestroyDebugUtilsMessengerEXT(VkInstance instance_,
                                     VkDebugUtilsMessengerEXT debugMessenger,
                                     const VkAllocationCallbacks* pAllocator);
  void TransitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout old_layout,
                             VkImageLayout new_layout);
  void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physical_device);
  bool CheckDeviceExtensionSupport(VkPhysicalDevice physical_device);

  SwapChainSupportDetails QuerySwapChainSupport(
      VkPhysicalDevice physical_device);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);
  VkShaderModule CreateShaderModule(const std::vector<char>& code);
  VkExtent2D ChooseSwapExtent(SDL_Window* window_,
                              const VkSurfaceCapabilitiesKHR& capabilities);
  VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  void UpdateUniformBuffer(uint32_t current_frame);
  std::vector<const char*> GetExtensions(SDL_Window* window_);
  void InitVulkan(gbengine::SDL* sdl, gbengine::Application app);
  void InitIMGUI(SDL* sdl);

  VkDescriptorSet descriptor_set;
  void _LoadImage(const char* image_path);
  void _FreeImage();
  void _DestoryImage();
  void CreateTextureDescriptorSet();

  void Chip8_Screen(std::array<std::array<uint32_t, 64>, 32>& screen);
  void GB_Screen(std::array<std::array<uint32_t, 160>, 144>& screen);

  void NES_Screen(std::array<std::array<uint32_t, 256>, 240>& screen);
};

}  // namespace gbengine