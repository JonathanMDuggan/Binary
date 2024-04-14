#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "../../drivers/include/renderer_vulkan.h"
#include "../../drivers/include/peripherals_sdl.h"

namespace gbengine {
  class GUI {
 public:
  GUI();
    void HelloIMGUI();
 private:
  SDL* sdl_;
  Vulkan* vulkan_; 
  void InitIMGUI();
  void LinkVulkanSDL2();
  ImGui_ImplVulkan_InitInfo init_info_ = {};
  };
}