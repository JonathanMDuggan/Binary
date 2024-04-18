#include "include/gb_gui.h"
namespace gbengine::gui::mainmenu {
void Start() {
  DrawMenuBar(); 
}

void DrawMenuBar() { 
  ImGui::NewFrame(); 
  if (ImGui::BeginMainMenuBar()) { 
    if (ImGui::BeginMenu("File")) { 
      if (ImGui::MenuItem("Open", "CTRL O")) { 
      }
      ImGui::EndMenu(); 
    }
  }
  ImGui::EndMainMenuBar(); 
  ImGui::Render(); 

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { 
    ImGui::UpdatePlatformWindows(); 
    ImGui::RenderPlatformWindowsDefault(); 
  }
}

}
