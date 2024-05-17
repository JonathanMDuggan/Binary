#include "include/gb_instruction.h"
#include <memory>

namespace retro::gb::instructionset {

// NOTE TO READER:
// machine cycle = 4 clock cycles
// The reason we use machine cycles is because every instruction uses more than
// four clock cycles, and if the machine requires more cycles, it's always
// a product of 4 
// 
// If an opcode is only one machine cycle it means function implementation
// doesn't increment the gb->cycle variable since the fetch function would
// do that anyways
//
// The only functions that increments gb->cycles are opcodes which have
// more than 1 machine cycle

void NoOperationFunction(GameBoy* gb) { return; }
void NoOperation(GameBoy* gb) { NoOperationFunction(gb); }

#define RETRO_GB_LOAD_REGISTER_FROM_REG(upper, lower)    \
  void LoadRegBFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.b_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegDFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.d_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegHFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegLFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegAFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegCFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_); \
  }

// Load Register Direct Functions Macros
RETRO_GB_LOAD_REGISTER_FROM_REG(B, b)
RETRO_GB_LOAD_REGISTER_FROM_REG(C, c)
RETRO_GB_LOAD_REGISTER_FROM_REG(D, d)
RETRO_GB_LOAD_REGISTER_FROM_REG(E, e)
RETRO_GB_LOAD_REGISTER_FROM_REG(H, h)
RETRO_GB_LOAD_REGISTER_FROM_REG(L, l)
inline void LoadRegisterDirect(uint8_t* reg, const uint8_t k_Reg){*reg = k_Reg;}
// Stop instruction
void Stop(GameBoy* gb) {
  // There's no reason to call a helper function for this, since there's only
  // one opcode like this
  gb->reg_.interrupt_ = false;
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

  (*opcode_table)[STOP].opcode = "00 10";
  (*opcode_table)[STOP].execute = Stop;
  (*opcode_table)[STOP].mnemonic = "STOP";

  (*opcode_table)[LD_B_B].opcode = "40";
  (*opcode_table)[LD_B_B].execute = LoadRegBFromRegB;
  (*opcode_table)[LD_B_B].mnemonic = "LD B,B";

  opcode_table_dst = *opcode_table;
}
}
