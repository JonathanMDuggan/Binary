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
    gb->UpdateRegBC();                                   \
  }                                                      \
  void LoadRegDFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.d_, gb->reg_.lower##_); \
    gb->UpdateRegDE();                                   \
  }                                                      \
  void LoadRegHFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_); \
    gb->UpdateRegHL();                                   \
  }                                                      \
  void LoadRegLFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.l_, gb->reg_.lower##_); \
    gb->UpdateRegHL();                                   \
  }                                                      \
  void LoadRegAFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.a_, gb->reg_.lower##_); \
  }                                                      \
  void LoadRegCFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.c_, gb->reg_.lower##_); \
    gb->UpdateRegBC();                                   \
  }                                                      \
  void LoadRegEFromReg##upper(GameBoy* gb) {             \
    LoadRegisterDirect(&gb->reg_.e_, gb->reg_.lower##_); \
    gb->UpdateRegDE();                                   \
  }
// Load Register Direct Functions Macros
RETRO_GB_LOAD_REGISTER_FROM_REG(B, b)
RETRO_GB_LOAD_REGISTER_FROM_REG(C, c)
RETRO_GB_LOAD_REGISTER_FROM_REG(D, d)
RETRO_GB_LOAD_REGISTER_FROM_REG(E, e)
RETRO_GB_LOAD_REGISTER_FROM_REG(H, h)
RETRO_GB_LOAD_REGISTER_FROM_REG(L, l)
RETRO_GB_LOAD_REGISTER_FROM_REG(A, a)
inline void LoadRegisterDirect(uint8_t* reg, const uint8_t k_Reg){*reg = k_Reg;}
// Stop instruction
void Stop(GameBoy* gb) {
  // There's no reason to call a helper function for this, since there's only
  // one opcode like this
  gb->reg_.interrupt_ = false;
}



}  // namespace retro::gb::instructionset

void retro::gb::GameBoy::ClearRegisters() { 
  reg_.a_  = 0;
  reg_.bc_ = 0;
  reg_.b_  = 0;
  reg_.c_  = 0;
  reg_.de_ = 0;
  reg_.d_  = 0;
  reg_.e_  = 0;
  reg_.f_  = 0;
  reg_.h_  = 0;
  reg_.l_  = 0;
  reg_.hl_ = 0;
}
void retro::gb::GameBoy::UpdateRegHL() {
  reg_.hl_ = (reg_.h_ << 8);
  reg_.hl_ |= reg_.l_;
}
void retro::gb::GameBoy::UpdateRegBC() {
  reg_.bc_ = (reg_.b_ << 8);
  reg_.bc_ |= reg_.c_;
}
void retro::gb::GameBoy::UpdateRegDE() {
  reg_.de_ = (reg_.d_ << 8);
  reg_.de_ |= reg_.e_;
}
void retro::gb::GameBoy::UpdateAll16BitReg() { 
  UpdateRegHL();
  UpdateRegBC();
  UpdateRegDE();
}
namespace retro::gb {

void InitOpcodeTable(std::array<Opcode, 512>& opcode_table_dst){
  using namespace retro::gb::instructionset;
  std::unique_ptr<std::array<Opcode, 512>>
  opcode_table = std::make_unique<std::array<Opcode, 512>>();

  (*opcode_table)[NOP].opcode = "00";
  (*opcode_table)[NOP].mnemonic = "NOP";
  (*opcode_table)[NOP].execute = NoOperation;
  (*opcode_table)[NOP].machine_cycles = 1;

  (*opcode_table)[STOP].opcode = "00 10";
  (*opcode_table)[STOP].mnemonic = "STOP";
  (*opcode_table)[STOP].execute = Stop;
  (*opcode_table)[STOP].machine_cycles = 2;

  (*opcode_table)[LD_B_B].opcode = "40";
  (*opcode_table)[LD_B_B].mnemonic = "LD B,B";
  (*opcode_table)[LD_B_B].execute = LoadRegBFromRegB;
  (*opcode_table)[LD_B_B].machine_cycles = 1;

  opcode_table_dst = std::move(*opcode_table);
}
}
