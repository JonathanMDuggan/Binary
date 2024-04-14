#pragma once
#include "include/gb_emulator.h" 
namespace gbengine::opcode{ 
void NOP_NoOperation_x00(Gameboy* gb) { 
  gb->sm83_.reg.program_counter_++; 
}
}