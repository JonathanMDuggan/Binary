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

#define GB_ENGINE_NAME "GameBoy Engine"
#define GB_VERSION 0

enum VulkanConst { 
  kFrameOverLap = 2
};

typedef struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
  bool IsComplete() {
    return graphics_family.has_value() && present_family.has_value();
  }
} QueueFamilyIndices;

// TODO: You should get infomation from a configure file
namespace gbengine {
extern std::vector<char> ReadFile(const std::string& filename);
extern std::string VkResultToString(VkResult result);

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

class GameBoyEngine {
  typedef struct SDL {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Surface* surface{};
    uint32_t* pCount{};
    const char** pNames{};
  } SDL;

  typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  } SwapChainSupportDetails;

  typedef struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer main_command_buffer{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};
    VkFence render_fence{};
  }FrameData;

  typedef struct SwapChain {
    VkSwapchainKHR KHR{};
    std::vector<VkImage> images{};
    std::vector<VkImageView> image_views{};
    std::vector<VkFramebuffer> frame_buffer;
    VkFormat image_format{};
    VkExtent2D extent{};
    SwapChainSupportDetails support_details = {};
  }SwapChain;

  typedef struct Vulkan {
    const char* const*       instance_validation_layers{};
    uint32_t                 sdl_extenstion_count{};
    const char**             kSDLExtensions{};
    VkApplicationInfo        app_info{};
    VkInstance               instance;
    VkPhysicalDevice         gpu{};
    VkQueueFamilyProperties* queue_props{};
    VkDebugUtilsMessengerEXT debug_messenger{};
    VkDevice                 device{};
    VkPhysicalDevice         physical_device{};
    VkDeviceCreateInfo       device_info{};
    VkSurfaceKHR             surface{};
    VkQueue                  queue{};
    VkDeviceQueueCreateInfo  queue_info{};
    VkCommandPool            command_pool{};
    VkCommandBuffer          main_command_buffer{};
    VkResult                 result{};
    SwapChain                swap_chain{};
    uint32_t                 graphics_queue_family{};
    FrameData                frame_data[kFrameOverLap] = {};
    VkPipelineLayout         pipeline_layout;
    VkRenderPass             render_pass;
    VkPipelineLayout         pipeline_layout;
    VkPipeline               pipeline;
    VkCommandBuffer          command_buffer
  }Vulkan;

  enum ResolutionValues {
    kStandardDefinitionWidth = 640,
    kStandardDefinitionHeight = 480
  };

  typedef struct Info {
    const char* name{};
    uint32_t version{};
  } Info;

  typedef struct Resolution {
    uint32_t width{};
    uint32_t height{};
  } Resolution;

  typedef struct Config {
    Resolution resolution{};
  } Config;

 private:
  void InitSDL();
  void InitVulkanApplication();
  void InitVulkan();
  void InitVulkanInfo();
  void InitVulkanPhysicalDevice();
  void InitVulkanValidationLayers();
  bool IsDeviceSuitable(VkPhysicalDevice physical_device);

  void InitVulkanInstance();
  void SetupDebugMessenger();
  void CreateSurface();
  void PickPhsycialDevice();
  void CreateLogicalDevice();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateGraphicsPipeline();
  void CreateRenderPass();
  void CreateFrameBuffer(); 
  void CreateCommandPool();
  void CreateCommandBuffer();
  void CreateSyncObjects();

  int RateDeviceSuitabillity(VkPhysicalDevice device);
  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& debug_info);
  bool VulkanValidationLayerSupported();

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);

  void DestroyDebugUtilsMessengerEXT(
      VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

 
  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);

  VkShaderModule CreateShaderModule(const std::vector<char>& code);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  std::vector<const char*> GetExtensions();
 public:
  Info app_info{};
  Config config{};
  SDL sdl{};
  Vulkan vk{};
  GameBoyEngine();
  void SDLPoolEvents(bool * running);
  ~GameBoyEngine();
};
}  // namespace gbengine
