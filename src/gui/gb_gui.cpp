#include "include/gb_gui.h"

static void IMGUI_CheckVkResult(VkResult result) {
  if (result == 0) {
    return;
  }
  //spdlog::error("IMGUI: VkResult {}\n", result);
  if (result < 0) {
    abort();
  }
}

void binary::GUI::StartGUI() { 
  spdlog::critical("GUI class is not pointing to a graphics API");
}

void binary::GUI::MainMenu() { 
  using namespace binary::gui::mainmenu;
  //DrawMenuBar();

}

void binary::VulkanGUI::StartGUI() {
  ImGui_ImplVulkan_NewFrame(); 
  ImGui_ImplSDL2_NewFrame();  
}

void binary::OpenGLGUI::StartGUI() { 
  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
}
 
void binary::OpenGL::InitIMGUI() {
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

  }
  auto sdl_gl_context = SDL_GL_CreateContext(sdl_->window_);
  ImGui_ImplSDL2_InitForOpenGL(sdl_->window_, sdl_gl_context);
  ImGui_ImplOpenGL3_Init();
  DefaultImGuiStyle();
}

void binary::Vulkan::InitIMGUI(SDL* sdl) {

  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info = {};

  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = std::size(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  vkCreateDescriptorPool(logical_device_, &pool_info, nullptr, &imgui_pool_);
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
    
  }

  ImGui_ImplSDL2_InitForVulkan(sdl->window_);
  ImGui_ImplVulkan_InitInfo imgui_info{};
  imgui_info.Instance = instance_; 
  imgui_info.PhysicalDevice = physical_device_;
  imgui_info.Device = logical_device_;
  imgui_info.QueueFamily = graphics_queue_family_; 
  imgui_info.Queue = graphics_queue_; 
  imgui_info.DescriptorPool = imgui_pool_;
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

void binary::DefaultImGuiStyle() {
  using namespace ImGui;
  ImGuiIO& io = GetIO(); 
  io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 16.0f);
  PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
  PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 7.0f));

}
