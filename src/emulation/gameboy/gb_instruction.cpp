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

#define RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(upper, lower)    \
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
    gb->UpdateRegAF();                                   \
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
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(B, b)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(C, c)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(D, d)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(E, e)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(H, h)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(L, l)
RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(A, a)

inline void LoadRegisterDirect(uint8_t* reg, const uint8_t k_Reg){*reg = k_Reg;}
void AddRegisterDirect8(uint8_t* reg, const uint8_t k_Reg, Flags* flag) {
  *reg += k_Reg;
  flag->zero_       = true;
  flag->half_carry_ = true;
  flag->carry_      = true;
}
inline void AddImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg += k_Operand;
  flag->zero_       = true;
  flag->half_carry_ = true;
  flag->carry_      = true;
}
inline void XorImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg ^= k_Operand;
  flag->zero_ = true;
}
inline void OrImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg |= k_Operand;
  flag->zero_ = true;
}
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
  Update16BitRegister(reg_.hl_, reg_.h_, reg_.l_);
}

void retro::gb::GameBoy::UpdateRegBC() {
  Update16BitRegister(reg_.bc_, reg_.b_, reg_.c_);
}

void retro::gb::GameBoy::UpdateRegDE() {
  Update16BitRegister(reg_.de_, reg_.d_, reg_.e_);
}

void retro::gb::GameBoy::UpdateRegAF() {
  Update16BitRegister(reg_.af_, reg_.a_, reg_.f_);
}

void retro::gb::GameBoy::UpdateFlags() {
  reg_.f_ = static_cast<uint8_t>(
    (flags_.zero_       << 8) | (flags_.subtract_ << 7) |
    (flags_.half_carry_ << 6) | (flags_.carry_ << 5));
}

void retro::gb::GameBoy::UpdateAll16BitReg() { 
  UpdateRegHL();
  UpdateRegBC();
  UpdateRegDE();
  UpdateRegAF();
}
void retro::gb::GameBoy::Update16BitRegister(uint16_t& _16bit_reg,
                                             const uint8_t& k_HighReg,
                                             const uint8_t& k_LowReg) {
  _16bit_reg = static_cast<uint16_t>((k_HighReg << 8) | k_LowReg);
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

  (*opcode_table)[LD_D_B].opcode = "50";
  (*opcode_table)[LD_D_B].mnemonic = "LD D,B";
  (*opcode_table)[LD_D_B].execute = LoadRegDFromRegB;
  (*opcode_table)[LD_D_B].machine_cycles = 1;

  (*opcode_table)[LD_H_B].opcode = "60";
  (*opcode_table)[LD_H_B].mnemonic = "LD H,B";
  (*opcode_table)[LD_H_B].execute = LoadRegHFromRegB;
  (*opcode_table)[LD_H_B].machine_cycles = 1;

  (*opcode_table)[ADD_A_B].opcode   = "80";
  (*opcode_table)[ADD_A_B].mnemonic = "ADD A,B";
  (*opcode_table)[ADD_A_B].execute  = LoadRegHFromRegB; // TODO: CHANGE TO ADD!
  (*opcode_table)[ADD_A_B].machine_cycles = 1;
  opcode_table_dst = std::move(*opcode_table);
}
}
