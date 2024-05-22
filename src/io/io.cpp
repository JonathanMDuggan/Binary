#pragma once
#include "include/io.h"
#include <yaml-cpp/yaml.h>

namespace binary {
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

Result LoadMainConfig(const std::string& file_path, Application* app) {

  if (app == nullptr) {
    spdlog::error("'app' was a nullptr: You must initialize 'app' before"
    "passing it to the {} function", __FUNCTION__);
    return k_FailedVarWasPassedAsNull;
  }

  if (file_path.empty()) {
    spdlog::error("'file_path' is empty: Did you forget to write the file path?");
    return k_FailedUninitializedVariable;
  }

  YAML::Node config = YAML::LoadFile(file_path);

  Application app_info{};
  // Name and Version
  app->name = config["application"]["name"].as<std::string>();
  app->version = config["application"]["version_number"].as<uint32_t>();

  // Window Settings
  app->height = config["window"]["height"].as<uint32_t>();
  app->width  = config["window"]["width"].as<uint32_t>();

  // Graphics API
  std::string graphics_api =
      config["graphic_api"]["name"].as<std::string>();

  if (graphics_api.compare("Vulkan") == 0){
    app->renderer = k_Vulkan;
  } else { // If it's not Vulkan then it must be OpenGL
    app->renderer = k_OpenGL;
  }
  return k_Success;
}
}  // namespace binary
