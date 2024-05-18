#pragma once
#include "include/io.h"
#include <yaml-cpp/yaml.h>
namespace retro {
std::vector<char> LoadRom(std::string file_path) {
  size_t rom_size;
  std::vector<char> ROM; 
  std::ifstream file(file_path, std::ios::ate | std::ios::binary);

  if (file.is_open() == false) {
    spdlog::error("Failed to load rom from path: {}", file_path);
    return ROM;
  }
  rom_size = static_cast<size_t>(file.tellg());
  ROM.resize(rom_size);
  file.seekg(0); 
  file.read(ROM.data(), rom_size);
  file.close();
  return ROM;
}

Application LoadMainConfig(const std::string& file_path) { 
  YAML::Node config = YAML::LoadFile(file_path);

  Application app_info{};
  // Name and Version
  app_info.name = config["application"]["name"].as<std::string>();
  app_info.version = config["application"]["version_number"].as<uint32_t>();

  // Window Settings
  app_info.height = config["window"]["height"].as<uint32_t>();
  app_info.width  = config["window"]["width"].as<uint32_t>();

  // Graphics API
  std::string graphics_api =
      config["graphic_api"]["name"].as<std::string>();

  if (graphics_api.compare("Vulkan") == 0){
    app_info.renderer = k_Vulkan;
  } else { // If it's not vulkan then it must be OpenGL
    app_info.renderer = k_OpenGL;
  }
  return app_info;
}
}  // namespace retro
