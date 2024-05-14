#include "include/gb_instruction.h" 
#include <iostream>

retro::gb::SM83::SM83() { 
  reg_.program_counter_ = (uint16_t)MemoryMap::ROM_BANK_00_START;
};
void retro::gb::SM83::PrintCurrentProgramCounterValue() {
  std::cout << reg_.program_counter_ << "\n";
}

uint32_t retro::gb::Test123() { 
  return 123; 
}
