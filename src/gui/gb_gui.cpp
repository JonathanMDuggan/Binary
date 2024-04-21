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

void gbengine::GUI::MainMenu() { 
  using namespace gbengine::gui::mainmenu;
  DrawMenuBar();

}

void gbengine::VulkanGUI::StartGUI() {
  ImGui_ImplVulkan_NewFrame(); 
  ImGui_ImplSDL2_NewFrame();  
}

void gbengine::OpenGLGUI::StartGUI() { 
  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
}
 
void gbengine::OpenGL::InitIMGUI() {
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
    style.Colors[ImGuiCol_WindowBg].w = 0.0f;
    style.Colors[ImGuiCol_MenuBarBg].w = 0.0f;
    style.Colors[ImGuiCol_Tab].w = 0.0f;
  }
  auto sdl_gl_context = SDL_GL_CreateContext(sdl_->window_);
  ImGui_ImplSDL2_InitForOpenGL(sdl_->window_, sdl_gl_context);
  ImGui_ImplOpenGL3_Init();
  DefaultImGuiStyle();
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
    style.Colors[ImGuiCol_WindowBg].w = 0.0f;
    style.Colors[ImGuiCol_MenuBarBg].w = 0.0f;
    style.Colors[ImGuiCol_Tab].w = 0.0f;
    
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
  DefaultImGuiStyle();

}

void gbengine::DefaultImGuiStyle() {
  using namespace ImGui;
  ImGuiIO& io = GetIO(); 
  io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 16.0f);
  PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
  PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 7.0f));

}
