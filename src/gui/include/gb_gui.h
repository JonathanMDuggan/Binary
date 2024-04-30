#include "../../drivers/include/renderer_vulkan.h"
#include "../../drivers/include/renderer_opengl.h"
#include "../../drivers/include/peripherals_sdl.h"
namespace gbengine {
class GUI {
 public: 
  virtual void StartGUI();
  virtual void MainMenu();
};
class VulkanGUI : public GUI {
  void StartGUI();
};

class OpenGLGUI : public GUI {
  void StartGUI();
};
extern void DefaultImGuiStyle();

class VulkanViewport {
 public:
  VulkanViewport(gbVulkanGraphicsHandler vulkan);
  void Destory();
  void Free();
  void LoadFromPath(const char* file_path);
  void LoadFromArray(void* array_data, VkDeviceSize array_size, uint32_t w,
                     uint32_t h);
 private:
  uint32_t mips_levels_{};
  VkSampler texture_sampler_{};
  VkImageView texture_image_view_{};
  VkDeviceMemory texture_image_memory_{};
  VkImage texture_image_{};

  // Pointers to vulkan logical device and its dependencies
  VkAllocationCallbacks* allocator_ = VK_NULL_HANDLE;
  VkDevice* logical_device_;
  VkQueue* graphics_queue_;
  VkCommandPool* command_pool_;
  VkDescriptorPool* descriptor_pool_;
  VkDescriptorSetLayout* descriptor_set_layout_;
};
}  // namespace gbengine

namespace gbengine::gui::mainmenu {
extern void Start(); 
extern void DrawMenuBar();
extern void Titles();
}
