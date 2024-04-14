#pragma once
#include "include/gb_emulator.h" 
#include "../io/include/io.h"

gbengine::Memory::Memory(std::string rom_file_path, SM83* sm83) { 
  const std::vector<char> ROM = LoadRom(rom_file_path); 
  for (size_t i = 0; i < ROM.size(); i++) {
    RAM_[i + kProgramStart] = static_cast<uint8_t>(ROM[i]);
  }
  sm83_ = sm83;
}

gbengine::Memory::~Memory() { return; }

uint8_t gbengine::Memory::ReadMemory() { 
  return RAM_[sm83_->reg.program_counter_];
}
