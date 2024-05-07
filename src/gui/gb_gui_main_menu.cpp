#include "include/gb_gui.h"
namespace retro::gui::mainmenu {
void Start(VulkanViewportInfo* texture) { 
  //
  ImGui::NewFrame(); 
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);
  DrawMenuBar(texture);
  Titles(texture); 

  ImGui::Render(); 
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
}

void DrawMenuBar(VulkanViewportInfo* texture) { 

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

void Titles(VulkanViewportInfo* texture) { 
   ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | 
                                   !ImGuiWindowFlags_NoDocking;
  if (ImGui::Begin("Screen View", nullptr, window_flags)) {
    VkDescriptorSet texture_descriptor_set;
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    texture_descriptor_set = (*texture->texture_descriptor_set);
    ImGui::Image((ImTextureID)texture_descriptor_set,
                 ImVec2(viewport_panel_size.x, viewport_panel_size.y));
  }
  ImGui::End();
}
}
