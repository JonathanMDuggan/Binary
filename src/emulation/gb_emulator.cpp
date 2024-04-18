#include "include/gb_emulator.h"
#include "include/gb.h"
void gbengine::test() { 
  gbengine::Gameboy gameboy;
  gameboy.sm83.PrintCurrentProgramCounterValue();
  return;
}