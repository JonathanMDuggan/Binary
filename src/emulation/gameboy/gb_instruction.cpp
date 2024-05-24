#include "include/gb_instruction.h"
#include <memory>

const uint8_t binary::gb::k_FlagZ = 0x80;
const uint8_t binary::gb::k_FlagN = 0x40;
const uint8_t binary::gb::k_FlagH = 0x20;
const uint8_t binary::gb::k_FlagC = 0x10;

namespace binary::gb::instructionset {

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

#define BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(upper, lower)            \
void LoadRegBFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.b_, gb->reg_.lower##_);                       \
  gb->UpdateRegBC();                                                         \
}                                                                            \
void LoadRegDFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.d_, gb->reg_.lower##_);                       \
  gb->UpdateRegDE();                                                         \
}                                                                            \
void LoadRegHFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.h_, gb->reg_.lower##_);                       \
  gb->UpdateRegHL();                                                         \
}                                                                            \
void LoadRegLFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.l_, gb->reg_.lower##_);                       \
  gb->UpdateRegHL();                                                         \
}                                                                            \
void LoadRegAFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.a_, gb->reg_.lower##_);                       \
  gb->UpdateRegAF();                                                         \
}                                                                            \
void LoadRegCFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.c_, gb->reg_.lower##_);                       \
  gb->UpdateRegBC();                                                         \
}                                                                            \
void LoadRegEFromReg##upper(GameBoy* gb) {                                   \
  LoadRegisterDirect(&gb->reg_.e_, gb->reg_.lower##_);                       \
  gb->UpdateRegDE();                                                         \
}                                                                            \
void AddRegAFromReg##upper(GameBoy* gb) {                                    \
  AddRegisterDirect8(gb->reg_.lower##_, gb);                                 \
}                                                                            \
void SubWithCarryRegAFromReg##upper(GameBoy* gb) {                           \
  SubWithCarryRegister(gb->reg_.lower##_, gb);                               \
}                                                                            \
void SubReg##upper(GameBoy* gb) {                                            \
  SubRegisterDirect8(gb->reg_.lower##_, gb);                                 \
}                                                                            \
void XorReg##upper(GameBoy* gb) {XorRegisterDirect8(gb->reg_.lower##_, gb);} \
void AndReg##upper(GameBoy* gb) {XorRegisterDirect8(gb->reg_.lower##_, gb);} \
void OrReg##upper(GameBoy* gb)  {OrRegisterDirect8(gb->reg_.lower##_, gb); } \
void CompareReg##upper(GameBoy* gb)  {OrRegisterDirect8(gb->reg_.lower##_, gb); } \

// Load Register Direct Functions Macros
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(B, b)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(C, c)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(D, d)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(E, e)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(H, h)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(L, l)
BINARY_GB_CREATE_8BIT_REG_ARITHMETIC_OPCODE(A, a)

inline void LoadRegisterDirect(uint8_t* reg, const uint8_t k_Operand){*reg = k_Operand;}
void LoadHighAddressIntoRegA(GameBoy* gb) {
  const uint8_t k_HighAddress = gb->memory_[gb->reg_.program_counter_];
  gb->reg_.a_ = k_HighAddress;
}
void LoadRegAIntoHighAddress(GameBoy* gb) {
  gb->memory_[gb->reg_.program_counter_] = gb->reg_.a_;
}
void AddRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ + k_Operand; 
  SetFlagZ0HC(gb, k_Result, &gb->reg_.a_, k_Operand);
  gb->reg_.a_ = k_Result;
}
inline void AddImmediate8(uint8_t* reg, const uint8_t k_Operand, Flags* flag) {

}

void XorRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ ^ k_Operand;
  gb->reg_.f_ = (k_Result != 0) ? false : k_FlagZ;  // SetFlagZ000
  gb->flags_.zero_ = static_cast<bool>(gb->reg_.f_);
  gb->reg_.a_ = k_Result;
}
void AndRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ & k_Operand;
  gb->reg_.f_ = (k_Result != 0) ? k_FlagH : (k_FlagZ | k_FlagH); // SetFlagZ010
  gb->reg_.a_ = k_Result;
}
void CompareRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ - k_Operand;
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, k_Operand);
  gb->reg_.f_ = k_Result;
}
void OrRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ | k_Operand;
  gb->reg_.f_ = (k_Result != 0) ? false : k_FlagZ;  // SetFlagZ000
  gb->flags_.zero_ = static_cast<bool>(gb->reg_.f_);
  gb->reg_.a_ = k_Result;
}
void SubWithCarryRegister(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.f_ - k_Operand - gb->flags_.carry_;
  gb->reg_.f_ = k_Result;
  SetFlagZ1HC(gb, k_Result, gb->reg_.f_, k_Operand);
}
void SubRegisterDirect8(const uint8_t k_Operand, GameBoy* gb) {
  const uint16_t k_Result = gb->reg_.a_ - k_Operand;
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, k_Operand);
  gb->reg_.a_ = k_Result;
}

void SubImmediate8Function(GameBoy* gb) {
  const uint16_t k_Operand = gb->memory_[gb->reg_.program_counter_ + 1];
  const uint16_t k_Result = gb->reg_.a_ - k_Operand;
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, k_Operand);
  gb->reg_.a_ = k_Result;
}

void SetFlagZ1HC(GameBoy* gb, const uint16_t k_Result, const uint8_t k_Reg,
                 const uint8_t k_Operand) {
  const bool k_IsZero   = (k_Result == 0);
  const bool k_IsHCarry = ((k_Reg & 0x0f) + (k_Operand & 0x0f) > 0x0f);
  const bool k_IsCarry  = ((k_Result & 0x100) != 0);

  gb->flags_.zero_      = k_IsZero;
  gb->flags_.subtract_  = true;
  gb->flags_.h_carry_   = k_IsHCarry;
  gb->flags_.carry_     = k_IsCarry;
  gb->UpdateFlags();
}
void SetFlagZ0HC(GameBoy* gb, const uint16_t k_Result, uint8_t* reg,
                 const uint8_t k_Operand) {
  const bool k_IsZero   = (k_Result == 0);
  const bool k_IsHCarry = ((*reg & 0x0f) + (k_Operand & 0x0f) > 0x0f);
  const bool k_IsCarry  = ((k_Result & 0x100) != 0);

  gb->flags_.zero_      = k_IsZero;
  gb->flags_.subtract_  = false;
  gb->flags_.h_carry_   = k_IsHCarry;
  gb->flags_.carry_     = k_IsCarry;
  gb->UpdateFlags();
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

void ReturnIfNotZero(GameBoy* gb) {
  if (!gb->flags_.zero_) ReturnFromSubRoutine(gb); 
}

void ReturnIfNotCarry(GameBoy* gb) {
  if (!gb->flags_.carry_) ReturnFromSubRoutine(gb); 
}

void ReturnIfCarry(GameBoy* gb) {
  if (gb->flags_.carry_) ReturnFromSubRoutine(gb); 
}

void ReturnIfZero(GameBoy* gb) {
  if (gb->flags_.zero_) ReturnFromSubRoutine(gb);
}
void ReturnFromSubRoutine(GameBoy* gb) {
  const uint8_t k_LowByte = gb->memory_[gb->reg_.stack_pointer_];
  const uint8_t k_HighByte = gb->memory_[gb->reg_.stack_pointer_ + 1];
  const uint16_t k_ReturnAddress = (k_HighByte << 8) | k_LowByte;
  gb->reg_.program_counter_ = k_ReturnAddress;
}
    // Stop instruction
void Stop(GameBoy* gb) {
  // There's no reason to call a helper function for this, since there's only
  // one opcode like this
  gb->reg_.interrupt_ = false;
}

}  // namespace binary::gb::instructionset

void binary::gb::Opcode::SetMachineCycles(uint8_t machine_cycles_branch,
                                         uint8_t machine_cycles) {
  machine_cycles_ = machine_cycles;
  machine_cycles_branch_ = machine_cycles_branch;
}

void binary::gb::Opcode::SetMachineCycles(uint8_t machine_cycles) {
  machine_cycles_ = machine_cycles;
  machine_cycles_branch_ = machine_cycles;
}

void binary::gb::GameBoy::ClearRegisters() { 
  reg_.a_  = 0;
  reg_.b_  = 0;
  reg_.c_  = 0;
  reg_.d_  = 0;
  reg_.e_  = 0;
  reg_.f_  = 0;
  reg_.h_  = 0;
  reg_.l_  = 0;
  UpdateAll16BitReg();
}


void binary::gb::GameBoy::UpdateRegHL() {
  Update16BitRegister(reg_.hl_, reg_.h_, reg_.l_);
}

void binary::gb::GameBoy::UpdateRegBC() {
  Update16BitRegister(reg_.bc_, reg_.b_, reg_.c_);
}

void binary::gb::GameBoy::UpdateRegDE() {
  Update16BitRegister(reg_.de_, reg_.d_, reg_.e_);
}

void binary::gb::GameBoy::UpdateRegAF() {
  Update16BitRegister(reg_.af_, reg_.a_, reg_.f_);
}


void binary::gb::GameBoy::UpdateAll16BitReg() { 
  UpdateRegHL();
  UpdateRegBC();
  UpdateRegDE();
  UpdateRegAF();
}

void binary::gb::GameBoy::SetFlags(CpuFlags Z, CpuFlags N,
                                  CpuFlags H, CpuFlags C) {

  // If the CpuFlag type equals k_Same , then set the value to
  // the current flag, else, set the value to the fuction parameters
  // ( 1 or 0 ).

  flags_.zero_     = (Z == k_Same) ? flags_.zero_     : static_cast<bool>(Z);
  flags_.carry_    = (C == k_Same) ? flags_.carry_    : static_cast<bool>(C);
  flags_.h_carry_  = (H == k_Same) ? flags_.h_carry_  : static_cast<bool>(H);
  flags_.subtract_ = (N == k_Same) ? flags_.subtract_ : static_cast<bool>(N);
  UpdateFlags();
}

void binary::gb::GameBoy::UpdateFlags() {
  reg_.f_ = static_cast<uint8_t>((flags_.zero_     << 8)| 
                                 (flags_.subtract_ << 7)|
                                 (flags_.h_carry_  << 6)|
                                 (flags_.carry_    << 5));
}
void binary::gb::GameBoy::Update16BitRegister(uint16_t& _16bit_reg,
                                             const uint8_t& k_HighReg,
                                             const uint8_t& k_LowReg) {
  _16bit_reg = static_cast<uint16_t>((k_HighReg << 8) | k_LowReg);
}

namespace binary::gb {

// :nerd_face: Functions cannot be >30 lines long! :nerd_face:
void InitOpcodeTable(std::array<Opcode, 512>& opcode_table){
  using namespace binary::gb::instructionset;

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
  opcode_table[ADD_A_B].execute_ = AddRegAFromRegB;
  opcode_table[ADD_A_B].SetMachineCycles(1);

  opcode_table[SUB_B].opcode_ = "90";
  opcode_table[SUB_B].mnemonic_ = "SUB B";
  opcode_table[SUB_B].execute_ = SubRegB;
  opcode_table[SUB_B].SetMachineCycles(1);

  opcode_table[AND_B].opcode_ = "A0";
  opcode_table[AND_B].mnemonic_ = "AND B";
  opcode_table[AND_B].execute_ = AndRegB;
  opcode_table[AND_B].SetMachineCycles(1);

  opcode_table[OR_B].opcode_ = "B0";
  opcode_table[OR_B].mnemonic_ = "OR B";
  opcode_table[OR_B].execute_ = OrRegB;
  opcode_table[OR_B].SetMachineCycles(1);

  opcode_table[RET_NZ].opcode_ = "C0";
  opcode_table[RET_NZ].mnemonic_ = "RET NZ";
  opcode_table[RET_NZ].execute_ = ReturnIfNotZero;
  opcode_table[RET_NZ].SetMachineCycles(5,2);

  opcode_table[RET_NC].opcode_ = "D0";
  opcode_table[RET_NC].mnemonic_ = "RET NC";
  opcode_table[RET_NC].execute_ = ReturnIfNotCarry;
  opcode_table[RET_NC].SetMachineCycles(5,2);

  opcode_table[AND_B].opcode_ = "E0";
  opcode_table[AND_B].mnemonic_ = "LDH %x00,A";
  opcode_table[AND_B].execute_ = LoadHighAddressIntoRegA;
  opcode_table[AND_B].SetMachineCycles(2);

  opcode_table[AND_B].opcode_ = "F0";
  opcode_table[AND_B].mnemonic_ = "LDH A, %x00";
  opcode_table[AND_B].execute_ = LoadRegAIntoHighAddress;
  opcode_table[AND_B].SetMachineCycles(2);
  
}
}
