#include "include/gb_gui.h"
gbengine::GUI::GUI(){
  InitIMGUI(); 
}

void gbengine::GUI::HelloIMGUI() { std::cout << "Hello ImGui! \n"; }

void gbengine::GUI::InitIMGUI() {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // Enables features like docking and taking the imgui windows outside the
  // renderer itself
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  ImGuiStyle& style = ImGui::GetStyle(); 

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { 
    style.WindowRounding = 0.0f; 
    style.Colors[ImGuiCol_WindowBg].w = 1.0f; 
  }

}

void gbengine::GUI::LinkVulkanSDL2(){
  ImGui_ImplSDL2_InitForVulkan(sdl_->window_);
}
