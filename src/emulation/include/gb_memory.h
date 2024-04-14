#pragma once
#include "gb_emulator.h"
#include <vector>
#include <array>
#include <string>
#include <cstdint>
namespace gbengine {
class Memory {
 public:
  Memory(std::string rom_file_path, SM83* sm83);
  ~Memory();
  uint8_t ReadMemory();
 private:
  SM83* sm83_;
  std::array<uint8_t, 0x4000> VRAM_;
  std::array<uint8_t, 0x8000> RAM_;
};
}

