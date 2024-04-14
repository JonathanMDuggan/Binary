#pragma once
#include "include/gb_emulator.h" 
#include <array>
gbengine::Error gbengine::InitOpcodeLoopUpTable( 
    std::array<InstructionSet, 512>* opcode_look_up_table) {
  // We set all the opcodes to ??? in case if the programmer forgot to add
  // one of them, 
  // the ??? means the opcode has not been worked on yet
  // the $ means the opcode has been declared but doesn't have all the functions
  // the ! means we defined the opcode but it isn't working as intended
  // Where there are no symbols next to the opcode that means the opcode seems
  // to be performing normally ( this can change when testing more roms)
  // needed to operate
  // 

  // Remove For Loop when All instructions have been defined 
  for (size_t i = 0; i < 512; i++) {
    (*opcode_look_up_table)[i].mnemonic = "???";
  }

  (*opcode_look_up_table)[0x00].mnemonic = "$NOP";
  (*opcode_look_up_table)[0x00].cycle_time.best_case = 1;
  (*opcode_look_up_table)[0x00].cycle_time.worst_case = 4;
  (*opcode_look_up_table)[0x00].prefix_cb = false;
  return GB_SUCCESS; 
}

gbengine::Error gbengine::OutputDisassembly(std::string output_path,
                                            std::vector<uint8_t> ROM) {
  if (ROM.empty()) {
    spdlog::error("ROM is empty");
    return GB_ROM_IS_EMPTY; 
  }
  const size_t rom_size = ROM.size();
  return GB_SUCCESS;
}

