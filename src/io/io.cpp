#pragma once
#include "include/io.h"
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
}  // namespace retro
