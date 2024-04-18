#include "include/gb_gui.h"

static void IMGUI_CheckVkResult(VkResult result) {
  if (result == 0) {
    return;
  }
  spdlog::error("IMGUI: VkResult {}\n", result);
  if (result < 0) {
    abort();
  }
}


void gbengine::GUI::StartGUI() { 
  spdlog::critical("GUI class is not pointing to a graphics API");
}

void gbengine::GUI::DrawMenuBar() {
  ImGui::NewFrame(); 
  ImGui::ShowDemoWindow(); 
  ImGui::Render(); 

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) { 
    ImGui::UpdatePlatformWindows(); 
    ImGui::RenderPlatformWindowsDefault(); 
  }
}

void gbengine::VulkanGUI::StartGUI() {
  ImGui_ImplVulkan_NewFrame(); 
  ImGui_ImplSDL2_NewFrame();  
}

void gbengine::OpenGLGUI::StartGUI() {

}
  
void gbengine::Vulkan::InitIMGUI(SDL* sdl) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

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

  ImGui_ImplSDL2_InitForVulkan(sdl->window_);
  ImGui_ImplVulkan_InitInfo imgui_info{};
  imgui_info.Instance = instance_; 
  imgui_info.PhysicalDevice = physical_device_;
  imgui_info.Device = logical_device_;
  imgui_info.QueueFamily = graphics_queue_family_; 
  imgui_info.Queue = graphics_queue_; 
  imgui_info.DescriptorPool = descriptor_pool_;
  imgui_info.RenderPass = render_pass_;
  imgui_info.Subpass = 0; 
  imgui_info.ImageCount = 3;
  imgui_info.MinImageCount = 3; 
  imgui_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT; 
  imgui_info.Allocator = allocator_; 
  imgui_info.CheckVkResultFn = IMGUI_CheckVkResult;
  ImGui_ImplVulkan_Init(&imgui_info);
}