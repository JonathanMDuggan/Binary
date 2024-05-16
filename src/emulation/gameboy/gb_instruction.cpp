#include "include/gb_instruction.h"
#include <memory>
namespace retro::gb::instructionset {

void NoOperationFunction(GameBoy* gb) { return; }
void NoOperation(GameBoy* gb) { NoOperationFunction(gb); }

// Load Register Direct Functions

void LoadRegBFromRegB(GameBoy* gb) {
  LoadRegisterDirect(&gb->sm83_.reg_.b_, gb->sm83_.reg_.b_);
}

void inline retro::gb::instructionset::LoadRegisterDirect(uint8_t* reg,
                                                          const uint8_t k_Reg) {
  // LD r, r;
  (*reg) = k_Reg;
}
}  // namespace retro::gb::instructionset
namespace retro::gb {

void InitOpcodeTable(std::array<Opcode, 512>& opcode_table_dst){
  using namespace retro::gb::instructionset;
  std::unique_ptr<std::array<Opcode, 512>> opcode_table(
      new std::array<Opcode, 512>);

  (*opcode_table)[NOP].opcode = "00";
  (*opcode_table)[NOP].execute = NoOperation;
  (*opcode_table)[NOP].mnemonic = "NOP";

  (*opcode_table)[LD_B_B].opcode = "40";
  (*opcode_table)[LD_B_B].execute = LoadRegBFromRegB;
  (*opcode_table)[LD_B_B].mnemonic = "LD B,B";

  opcode_table_dst = *opcode_table;
}
}
