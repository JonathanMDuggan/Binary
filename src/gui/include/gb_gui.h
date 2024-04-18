#include "../../drivers/include/renderer_vulkan.h"
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
}  // namespace gbengine

namespace gbengine::gui::mainmenu {
extern void Start(); 
extern void DrawMenuBar();
}
