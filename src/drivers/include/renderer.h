#pragma once 
#include "peripherals_sdl.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace binary {
// Vulkan graphic device commuication struct
typedef struct gbVulkanGraphicsHandler {
  VkPhysicalDevice* physical_device;
  VkDevice* logical_device;
  VkQueue* graphics_queue;
  VkCommandPool* command_pool;
  VkDescriptorPool* descriptor_pool;
  VkDescriptorPool* imgui_pool; 
  VkDescriptorSetLayout* descriptor_set_layout;
} gbVulkanGraphicsHandler;

class Renderer {
 public:
  virtual void DrawFrame();
  virtual void StartIMGUI();
  virtual gbVulkanGraphicsHandler GetGraphicsHandler();
  virtual ~Renderer();
};
}
