#include "include/gb_cpu.h"
#include "include/gb_memory.h"
#include <iostream>
gbengine::SM83::SM83() { register_.program_counter_ = PROGRAM_START; }
void gbengine::SM83::PrintCurrentProgramCounterValue() {
  std::cout << register_.program_counter_ << "\n";
}