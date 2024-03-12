#pragma once
#include <string>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>
#include <SDL.h>
#include <vulkan/vulkan.h>
#define GB_ENGINE_NAME "GameBoy Engine"
#define GB_VERSION 0

// TODO: You should get infomation from a configure file
namespace gbengine {

typedef struct SDL {
  SDL_Window*   window;
  SDL_Renderer* renderer;
  SDL_Surface*  surface;
  uint32_t* pCount;
  const char** pNames;
} SDL;

typedef struct Vulkan {
  const char* const*       instance_validation_layers;
  uint32_t                 sdl_extenstion_count;
  const char**             kSDLExtensions;
  VkApplicationInfo        info;
  VkInstanceCreateInfo     instance_info;
  VkInstance*              inst;
  VkPhysicalDevice*        physical_devices;
  VkQueueFamilyProperties* queue_props;
  VkDevice                 device;
  VkDeviceQueueCreateInfo  queue;
  VkDeviceCreateInfo       device;
  VkSurfaceKHR             surface;
  VkQueue                  queue;
  VkResult                 result;
};

enum ResolutionValues {
  kStandardDefinitionWidth = 640,
  kStandardDefinitionHeight = 480
};

typedef struct Info {
  const char* name;
  uint32_t version;
};

typedef struct Resolution {
  uint32_t width;
  uint32_t height;
};

typedef struct Config {
  Resolution resolution;
};

class GameBoyEngine {
 private:
  uint32_t InitSDL();
  uint32_t InitVulkan();
  uint32_t InitSwapChain();
  uint32_t initCommands();

 public:
  Info info;
  Config config;
  SDL sdl;
  Vulkan vulkan;
  GameBoyEngine(uint32_t mode_flags);
};
