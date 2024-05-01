#include "../include/gb_emulator.h"
#include "../include/gb.h"
void retro::gb::test() {
  retro::gb::Gameboy gameboy;
  gameboy.sm83.PrintCurrentProgramCounterValue();
  return;
}