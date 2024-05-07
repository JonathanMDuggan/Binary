#include "include/gb_cpu.h"
#include "include/gb_memory.h"
#include <iostream>

retro::gb::SM83::SM83() { register_.program_counter_ = PROGRAM_START; }
void retro::gb::SM83::PrintCurrentProgramCounterValue() {
  std::cout << register_.program_counter_ << "\n";
}

uint32_t retro::gb::Test123() { 
  return 123; 
}
