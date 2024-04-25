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
 private:
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
