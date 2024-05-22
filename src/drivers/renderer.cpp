#include "include/renderer.h" 
#include <iostream>
void binary::Renderer::DrawFrame() { std::cout << "bazinga\n"; }
binary::Renderer::~Renderer() { std::cout << "bazinga\n"; }
void binary::Renderer::StartIMGUI() { std::cout << "There is no renderer!"; }

binary::gbVulkanGraphicsHandler binary::Renderer::GetGraphicsHandler() { 
  gbVulkanGraphicsHandler graphics_handler{};
  std::cout << "Vulkan wasn't derived\n"; 
  throw std::runtime_error("Vulkan wasn't derived\n");
  return graphics_handler;
}
