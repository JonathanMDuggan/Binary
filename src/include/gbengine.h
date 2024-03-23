#pragma once
#include <string>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include <optional>


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
#define VK_CHECK(x)                                               \
  do {                                                            \
    VkResult err = x;                                             \
    if (err) {                                                    \
      std::cout << "Detected Vulkan error: " << err << std::endl; \
      abort();                                                    \
    }                                                             \
  } while (0)

enum VulkanConst { 
  kFrameOverLap = 2
};

typedef struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  bool IsComplete() { return graphics_family.has_value();
  }
} QueueFamilyIndices;

// TODO: You should get infomation from a configure file
namespace gbengine {
const std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};

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


  typedef struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer main_command_buffer{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};
    VkFence render_fence{};
  }FrameData;
  
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
    VkSwapchainKHR           swapchain{}; 
    std::vector<VkImage>     swapchain_images{};
    std::vector<VkImageView> swapchain_image_views{};
    VkFormat                 swapchain_image_format{};
    uint32_t                 graphics_queue_family{};
    FrameData                frame_data[kFrameOverLap] = {};
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
  void InitVulkan();
  void InitVulkanApplication();
  void InitVulkanInfo();
  void InitVulkanInstance();
  void InitVulkanPhysicalDevice();
  void InitVulkanValidationLayers();
  void PickPhsycialDevice();
  bool IsDeviceSuitable(VkPhysicalDevice physical_device);
  void SetupDebugMessenger(); 

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
      VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

  void CreateLogicalDevice();
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
