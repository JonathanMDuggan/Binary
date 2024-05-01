#include "include/gb_gui.h"
namespace retro::gui::mainmenu {
void Start() {
  //
  ImGui::NewFrame(); 
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);
  DrawMenuBar(); 
  Titles(); 

  ImGui::Render(); 
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

}

void DrawMenuBar() { 

  if (ImGui::BeginMainMenuBar()) { 
    if (ImGui::BeginMenu("File")) { 
      if (ImGui::MenuItem("New", "CTRL O")) {
      }
      if (ImGui::MenuItem("Open", "CTRL O")) {
      }
      if (ImGui::MenuItem("Export", "CTRL O")) {
      }
      ImGui::EndMenu(); 
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Bazinga", "CTRL O")) {
      }
      ImGui::EndMenu(); 
    }
    if (ImGui::BeginMenu("Options")) {
      if (ImGui::MenuItem("Bazinga", "CTRL O")) {
      }
      ImGui::EndMenu(); 
    }
  }
  ImGui::EndMainMenuBar(); 
}

void Titles() { 
   ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | 
                                   !ImGuiWindowFlags_NoDocking;
  if (ImGui::Begin("Start Window", nullptr, window_flags)) {
  }
  ImGui::End();
}

}
