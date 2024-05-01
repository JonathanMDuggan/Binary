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
  void Update(void* array_data);
  void LoadFromPath(const char* file_path);
  void LoadFromArray(void* array_data, VkDeviceSize array_size, uint32_t w,
                     uint32_t h);

  VkDescriptorSet descriptor_set_;  
 private:
  uint32_t mips_levels_{};
  VkSampler texture_sampler_{};
  VkImageView texture_image_view_{};
  VkDeviceMemory texture_image_memory_{};
  VkImage texture_image_{};
  VkDeviceSize array_size_;
  uint32_t w_;
  uint32_t h_;

  // Pointers to vulkan logical device and its dependencies 
  std::unique_ptr<VkAllocationCallbacks> allocator_ = VK_NULL_HANDLE;
  std::unique_ptr<VkDevice> logical_device_;
  std::unique_ptr<VkQueue> graphics_queue_;
  std::unique_ptr<VkCommandPool> command_pool_;
  std::unique_ptr<VkDescriptorPool> descriptor_pool_;
  std::unique_ptr<VkDescriptorSetLayout> descriptor_set_layout_;
  std::unique_ptr<VkDescriptorSetLayout> texture_descriptor_set_layout;
};
}  // namespace gbengine

namespace gbengine::gui::mainmenu {
extern void Start(); 
extern void DrawMenuBar();
extern void Titles();
}
