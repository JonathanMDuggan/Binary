#include "include/renderer.h" 
#include <iostream>
void retro::Renderer::DrawFrame() { std::cout << "bazinga\n"; }
retro::Renderer::~Renderer() { std::cout << "bazinga\n"; }
void retro::Renderer::StartIMGUI() { std::cout << "There is no renderer!"; }

retro::gbVulkanGraphicsHandler retro::Renderer::GetGraphicsHandler() { 
  gbVulkanGraphicsHandler graphics_handler{};
  std::cout << "Vulkan wasn't derived\n"; 
  throw std::runtime_error("Vulkan wasn't derived\n");
  return graphics_handler;
}
