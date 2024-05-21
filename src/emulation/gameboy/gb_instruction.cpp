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

#define RETRO_GB_CREATE_8BIT_REG_ARITHMETIC_FUNCTIONS(upper, lower) \
void LoadRegBFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.b_, gb->reg_.lower##_);              \
  gb->UpdateRegBC();                                                \
}                                                                   \
void LoadRegDFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.d_, gb->reg_.lower##_);              \
  gb->UpdateRegDE();                                                \
}                                                                   \
void LoadRegHFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_);              \
  gb->UpdateRegHL();                                                \
}                                                                   \
void LoadRegLFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.l_, gb->reg_.lower##_);              \
  gb->UpdateRegHL();                                                \
}                                                                   \
void LoadRegAFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.a_, gb->reg_.lower##_);              \
  gb->UpdateRegAF();                                                \
}                                                                   \
void LoadRegCFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.c_, gb->reg_.lower##_);              \
  gb->UpdateRegBC();                                                \
}                                                                   \
void LoadRegEFromReg##upper(GameBoy* gb) {                          \
  LoadRegisterDirect(&gb->reg_.e_, gb->reg_.lower##_);              \
  gb->UpdateRegDE();                                                \
}                                                                   \
void AddRegAFromReg##upper(GameBoy* gb) {                           \
  AddRegisterDirect8(&gb->reg_.a_, gb->reg_.lower##_, gb);          \
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

void AddRegisterDirect8(uint8_t* reg, const uint8_t k_Reg, GameBoy* gb) {
  const uint16_t k_Result = *reg + k_Reg;
  gb->flags_.zero_     = (k_Result == 0);
  gb->flags_.subtract_ = false;
  gb->flags_.hcarry_   = ((*reg & 0x0f) + (k_Reg & 0x0f) > 0x0f);
  gb->flags_.carry_    = ((k_Result & 0x100) != 0);
  *reg += k_Reg;
}

void SubWithCarryRegister(uint8_t* reg, const uint8_t k_Reg, GameBoy* gb) {
  const uint16_t k_Result = *reg - k_Reg;
  SetFlagZ1HC(gb, k_Result, reg, k_Reg);
  *reg -= k_Reg;
}

void SubImmediate8Function(uint8_t* reg, GameBoy* gb) {
  const uint16_t k_Operand = gb->memory_[gb->reg_.program_counter_ + 1];
  const uint16_t k_Result  = *reg - k_Operand;
  SetFlagZ1HC(gb, k_Result, reg, k_Operand);
  gb->reg_.a_ = k_Result;
}

void SetFlagZ1HC(GameBoy* gb, const uint16_t k_Result, uint8_t* reg,
                 const uint8_t k_Operand) {
  gb->flags_.zero_ = (k_Result == 0);
  gb->flags_.subtract_ = true;
  gb->flags_.hcarry_ = ((*reg & 0x0f) + (k_Operand & 0x0f) > 0x0f);
  gb->flags_.carry_ = ((k_Result & 0x100) != 0);
}

inline void AddImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg += k_Operand;
}
inline void XorImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg ^= k_Operand;
  flag->zero_ = true;
}
inline void OrImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {
  *reg |= k_Operand;
  flag->zero_ = true;
}
void JumpRelativeSignedIfNotZero(GameBoy* gb) {
  if (!gb->flags_.zero_) {
    JumpRelative(gb);
  }
}

void JumpRelativeSignedIfNotCarry(GameBoy* gb) {
  if (!gb->flags_.carry_) {
    JumpRelative(gb);
  }
}

void JumpRelative(GameBoy* gb) {
  gb->reg_.program_counter_ += gb->memory_[gb->reg_.program_counter_ + 1];
  gb->branched = true;
}
// Stop instruction
void Stop(GameBoy* gb) {
  // There's no reason to call a helper function for this, since there's only
  // one opcode like this
  gb->reg_.interrupt_ = false;
}

}  // namespace retro::gb::instructionset

void retro::gb::Opcode::SetMachineCycles(uint8_t machine_cycles_branch,
                                         uint8_t machine_cycles) {
  machine_cycles_ = machine_cycles;
  machine_cycles_branch_ = machine_cycles_branch;
}

void retro::gb::Opcode::SetMachineCycles(uint8_t machine_cycles) {
  machine_cycles_ = machine_cycles;
  machine_cycles_branch_ = machine_cycles;
}

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


void retro::gb::GameBoy::UpdateAll16BitReg() { 
  UpdateRegHL();
  UpdateRegBC();
  UpdateRegDE();
  UpdateRegAF();
}

void retro::gb::GameBoy::SetFlags(CpuFlags Z, CpuFlags N,
                                  CpuFlags H, CpuFlags C) {

  // If the CpuFlag type equals k_Same , then set the value to
  // the current flag, else, set the value to the fuction parameters
  // ( 1 or 0 ).

  flags_.zero_     = (Z == k_Same) ? flags_.zero_     : static_cast<bool>(Z);
  flags_.carry_    = (C == k_Same) ? flags_.carry_    : static_cast<bool>(C);
  flags_.hcarry_   = (H == k_Same) ? flags_.hcarry_   : static_cast<bool>(H);
  flags_.subtract_ = (N == k_Same) ? flags_.subtract_ : static_cast<bool>(N);
  UpdateFlags();
}

void retro::gb::GameBoy::UpdateFlags() {
  reg_.f_ = static_cast<uint8_t>((flags_.zero_     << 8)| 
                                 (flags_.subtract_ << 7)|
                                 (flags_.hcarry_   << 6)|
                                 (flags_.carry_    << 5));
}
void retro::gb::GameBoy::Update16BitRegister(uint16_t& _16bit_reg,
                                             const uint8_t& k_HighReg,
                                             const uint8_t& k_LowReg) {
  _16bit_reg = static_cast<uint16_t>((k_HighReg << 8) | k_LowReg);
}

namespace retro::gb {

void InitOpcodeTable(std::array<Opcode, 512>& opcode_table){
  using namespace retro::gb::instructionset;

  opcode_table[NOP].opcode_ = "00";
  opcode_table[NOP].mnemonic_ = "NOP";
  opcode_table[NOP].execute_ = NoOperation;
  opcode_table[NOP].SetMachineCycles(1);

  opcode_table[STOP].opcode_ = "00 10";
  opcode_table[STOP].mnemonic_ = "STOP";
  opcode_table[STOP].execute_ = Stop;
  opcode_table[STOP].SetMachineCycles(2);

  opcode_table[JR_NZ_R8].opcode_ = "20 %x00";
  opcode_table[JR_NZ_R8].mnemonic_ = "JR NZ,";
  opcode_table[JR_NZ_R8].execute_ = JumpRelativeSignedIfNotZero;
  opcode_table[JR_NZ_R8].SetMachineCycles(12,8);

  opcode_table[JR_NC_R8].opcode_ = "30 %x00";
  opcode_table[JR_NC_R8].mnemonic_ = "JR NZ,";
  opcode_table[JR_NC_R8].execute_ = JumpRelativeSignedIfNotCarry;
  opcode_table[JR_NC_R8].SetMachineCycles(12,8);

  opcode_table[LD_B_B].opcode_ = "40";
  opcode_table[LD_B_B].mnemonic_ = "LD B,B";
  opcode_table[LD_B_B].execute_ = LoadRegBFromRegB;
  opcode_table[LD_B_B].SetMachineCycles(1);

  opcode_table[LD_D_B].opcode_ = "50";
  opcode_table[LD_D_B].mnemonic_ = "LD D,B";
  opcode_table[LD_D_B].execute_ = LoadRegDFromRegB;
  opcode_table[LD_D_B].SetMachineCycles(1);

  opcode_table[LD_H_B].opcode_ = "60";
  opcode_table[LD_H_B].mnemonic_ = "LD H,B";
  opcode_table[LD_H_B].execute_ = LoadRegHFromRegB;
  opcode_table[LD_H_B].SetMachineCycles(1);

  opcode_table[ADD_A_B].opcode_   = "80";
  opcode_table[ADD_A_B].mnemonic_ = "ADD A,B";
  opcode_table[ADD_A_B].execute_ = AddRegAFromRegB; // TODO: CHANGE TO ADD!
  opcode_table[ADD_A_B].SetMachineCycles(1);
}
}
