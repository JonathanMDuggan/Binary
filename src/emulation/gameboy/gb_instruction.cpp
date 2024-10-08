#include "include/gb_instruction.h"
#include <memory>
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

void NoOperation(GameBoy* gb) { return; }

void binary::gb::instructionset::ReturnFromInterruptHandler(GameBoy* gb) {
  gb->reg_.program_counter_ = gb->ReturnAddress();
  gb->reg_.interrupt_ = 1;
}

void JumpIndirect(GameBoy* gb) {
  const uint16_t k_JumpAddress = gb->memory_[gb->reg_.hl_];
  gb->reg_.stack_pointer_ = gb->reg_.program_counter_;
  gb->reg_.program_counter_ = k_JumpAddress;
}

void LoadHighAddressIntoRegA(GameBoy* gb) {
  const uint8_t k_HighAddress = gb->memory_[gb->reg_.program_counter_ + 2];
  gb->reg_.a_ = k_HighAddress;
}
void LoadRegAIntoHighAddress(GameBoy* gb) {
  gb->memory_[gb->reg_.program_counter_] = gb->reg_.a_;
}

void SubImmediate8Function(GameBoy* gb) {
  const uint16_t k_Operand = gb->memory_[gb->reg_.program_counter_ + 1];
  const uint16_t k_Result = gb->reg_.a_ - k_Operand;
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, k_Operand);
  gb->reg_.a_ = k_Result;
}

void SetFlagZ1HC(GameBoy* gb, const uint16_t k_Result, const uint8_t k_Reg,
                 const uint8_t k_Operand) {
  const bool k_IsZero   = (static_cast<uint8_t>(k_Result) == 0);
  const bool k_IsHCarry = ((k_Reg & 0x0f) + (k_Operand & 0x0f) > 0x0f);
  const bool k_IsCarry  = ((k_Result & 0x100) != 0);

  gb->reg_.f_[k_BitIndexZ] = k_IsZero;
  gb->reg_.f_[k_BitIndexN] = true;
  gb->reg_.f_[k_BitIndexH] = k_IsHCarry;
  gb->reg_.f_[k_BitIndexC] = k_IsCarry;
}

void SetFlagZ00C(GameBoy* gb, const uint16_t k_Result) {
  const bool k_IsZero = (static_cast<uint8_t>(k_Result) == 0);
  const bool k_IsCarry = ((k_Result & 0x100) != 0);
  gb->reg_.f_[k_BitIndexZ] = k_IsZero;
  gb->reg_.f_[k_BitIndexN] = false;
  gb->reg_.f_[k_BitIndexH] = false;
  gb->reg_.f_[k_BitIndexC] = k_IsCarry;
}

void PrefixCB(GameBoy* gb) {
  gb->cb_prefixed = true;
  gb->reg_.program_counter_++;
  return;
}

void EnableInterrput(GameBoy* gb) { gb->reg_.interrupt_ = true; }

void DisableInterrput(GameBoy* gb) { gb->reg_.interrupt_ = false; }

void SetCarryFlag(GameBoy* gb) { 
  gb->reg_.f_[k_BitIndexN] = false;
  gb->reg_.f_[k_BitIndexH] = false;
  gb->reg_.f_[k_BitIndexC] = true; 
  gb->UpdateRegisters<&Register::a_>();
}

void ComplementCarryFlag(GameBoy* gb) {
  gb->reg_.f_[k_BitIndexN] = false;
  gb->reg_.f_[k_BitIndexH] = false;
  gb->reg_.f_[k_BitIndexC] = ~gb->reg_.f_[k_BitIndexC];
  gb->UpdateRegisters<&Register::a_>();
}

void ComplementAccumulator(GameBoy* gb) {
  gb->reg_.f_[k_BitIndexN] = true;
  gb->reg_.f_[k_BitIndexH] = true;
  gb->reg_.a_ = ~gb->reg_.a_;
  gb->UpdateRegisters<&Register::a_>();
}

void RotateLeftAccumulatorCarry(GameBoy* gb) {
  const uint8_t k_MostSignificantBit = ((gb->reg_.a_ & 0b01111111) != 0);
  const bool k_IsCarryFlagSet = gb->reg_.f_[k_BitIndexC];
  const uint8_t k_Result = (gb->reg_.a_ << 1) | gb->reg_.f_[k_BitIndexC];

  SetFlagZ00C(gb, k_Result);
  gb->reg_.f_[k_BitIndexC] = k_MostSignificantBit;
  gb->reg_.a_ = k_Result;
  gb->UpdateRegisters<&Register::a_>();
}

void RotateLeftAccumulator(GameBoy* gb) {
  const uint8_t k_MostSignificantBit = ((gb->reg_.a_ & 0b01111111) != 0);
  const uint8_t k_Result = (gb->reg_.a_ << 1) | k_MostSignificantBit;

  SetFlagZ00C(gb, k_Result); 
  gb->reg_.a_ = k_Result; 
  gb->UpdateRegisters<&Register::a_>();
}

void RotateRightAccumulatorCarry(GameBoy* gb) {
  bool k_FirstBit = ((gb->reg_.a_ & 1) > 0);
  gb->reg_.a_ >>= 1;
  gb->reg_.a_ |= (k_FirstBit == true) ? 0x80 : 0;
  gb->reg_.f_[k_BitIndexC] = k_FirstBit;
  gb->reg_.f_[k_BitIndexH] = false;
  gb->UpdateRegisters<&Register::a_>();
}

void RotateRightAccumulator(GameBoy* gb) {
  bool k_FirstBit = ((gb->reg_.a_ & 1) > 0);
  gb->reg_.a_ >>= 1; 
  gb->reg_.a_ |= (k_FirstBit == true) ? 0x80 : 0;
  gb->UpdateRegisters<&Register::a_>();
}



void SetFlagZ0HC(GameBoy* gb, const uint16_t k_Result, uint8_t reg,
                 const uint8_t k_Operand) {
  const bool k_IsZero = (static_cast<uint8_t>(k_Result) == 0);
  const bool k_IsHCarry = ((reg & 0x0f) + (k_Operand & 0x0f) > 0x0f);
  const bool k_IsCarry  = ((k_Result & 0x100) != 0);

  gb->reg_.f_[k_BitIndexZ] = k_IsZero;
  gb->reg_.f_[k_BitIndexN] = false;
  gb->reg_.f_[k_BitIndexH] = k_IsHCarry;
  gb->reg_.f_[k_BitIndexC] = k_IsCarry;
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
  const uint8_t k_RegisterFValue = static_cast<uint8_t>(reg_.f_.to_ulong());
  Update16BitRegister(reg_.af_, reg_.a_, k_RegisterFValue);
}

void binary::gb::GameBoy::UpdateAll16BitReg() { 
  UpdateRegHL();
  UpdateRegBC();
  UpdateRegDE();
  UpdateRegAF();
}

uint8_t binary::gb::GameBoy::Operand8Bit() {
  return memory_[reg_.program_counter_ + 1];
}
uint16_t binary::gb::GameBoy::Operand16Bit() { 
  const uint16_t k_LowByte = memory_[reg_.program_counter_ + 1];
  const uint16_t k_HighByte = memory_[reg_.program_counter_ + 2];
  const uint16_t k_Operand16bit = (k_HighByte << 8) | k_LowByte;
  return k_Operand16bit;
}
uint16_t binary::gb::GameBoy::ReturnAddress() { 
  const uint16_t k_LowByte = memory_[reg_.stack_pointer_ + 1];
  const uint16_t k_HighByte = memory_[reg_.stack_pointer_ + 2];
  const uint16_t k_ReturnAddress = (k_HighByte << 8) | k_LowByte;
  reg_.stack_pointer_ += 2;
  return k_ReturnAddress;

}
void binary::gb::GameBoy::GetProgramCounterBytes(uint8_t& high_byte,
                                              uint8_t& low_byte) {
   high_byte = (reg_.program_counter_ >> 8);
   low_byte =  (reg_.program_counter_ << 8);
   return;
}
void binary::gb::GameBoy::Update16BitRegister(uint16_t& _16bit_reg,
                                             const uint8_t& k_HighReg,
                                             const uint8_t& k_LowReg) {
  _16bit_reg = static_cast<uint16_t>((k_HighReg << 8) | k_LowReg);
}

namespace binary::gb {

void InitOpcodeTable(std::array<Opcode, 512>& opcode_table) {
  InitLoadInstructionsTable(opcode_table);
  Init8BitArithmeticLogicRegisterDirectTable(opcode_table);
  InitIncrementAndDecrement(opcode_table);
  InitConditional(opcode_table);
  InitPushAndPop(opcode_table);
  InitRestart(opcode_table);
  InitPrefixTable(opcode_table); 
  InitNullOpcodes(opcode_table);
  InitMiscellaneous(opcode_table);
}

void InitPrefixTable(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "(HL)", "A"};
  const std::array<std::string, 32> k_Mnemonic = {
  "RLC",    "RRC",   "RL",     "RR",     "SLA",    "SRA",    "SWAP",   "SRL", 
  "BIT 0,", "BIT 1," "BIT 2,", "BIT 3,", "BIT 4," ,"BIT 5,", "BIT 6,", "BIT 7,",
  "RES 0," ,"RES 1," "RES 2,", "RES 3,", "RES 4,", "RES 5,", "RES 6,", "RES 7,",
  "SET 0,", "SET 1," "SET 2,", "SET 3,", "SET 4,", "SET 5,", "SET 6,", "SET 7,"
  };
  uint8_t mnemonic = 0;

  for (size_t opcode = 0x100; opcode < 0x200; opcode++) {
    // Skip the indirect instructions
    if (((~opcode & 0x6) == 0) || ((~opcode & 0xE) == 0)) {
      opcode_table[opcode].SetMachineCycles(4); 
    } else {
      opcode_table[opcode].SetMachineCycles(2); 
    }
    opcode_table[opcode].opcode_ = PrintOpcode<>;
    opcode_table[opcode].mnemonic_ =
        std::format("{} {}", k_Mnemonic[mnemonic], k_Letter[opcode % 8]);

    if (((opcode + 1) % 8) == 0) {
      mnemonic++;
    }
  }
  BINARY_GB_ALL_REG(BINARY_GB_EXECUTE_BYTE_PREFIX);
  BINARY_GB_REPEAT_FOR_ALL_BIT_PREFIX(BINARY_GB_EXECUTE_BIT_PREFIX);
  BINARY_GB_EXECUTE_REGISTER_INDIRECT_BYTE_PREFIX;
  BINARY_GB_REPEAT_FOR_ALL_REGISTER_INDIRECT_BIT_PREFIX(
      BINARY_GB_EXECUTE_REGISTER_INDIRECT_BIT_PREFIX);
}

void Init8BitArithmeticLogicRegisterDirectTable(
    std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  const std::array<std::string, 8> k_RegisterNames = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  const std::array<std::string, 8> k_Mnemonics = {"ADD", "ADC", "SUB", "SBC",
                                                 "AND", "XOR", "OR",  "CP"};
  uint8_t mnemonic = 0;
  for (size_t opcode = 0x80; opcode < 0xC0; opcode++) {
    // Skip the indirect instructions
    if (((~opcode & 0x6) == 0) || ((~opcode & 0xE) == 0)) {
      continue;
    }

    opcode_table[opcode].SetMachineCycles(1);
    opcode_table[opcode].opcode_ = PrintOpcode<>;
    opcode_table[opcode].mnemonic_ =
        std::format("{} {}", k_Mnemonics[mnemonic], k_RegisterNames[opcode % 8]);
    if (((opcode + 1) % 8) == 0) {
      mnemonic++;
    }
  }
  InitGenericOpcode<2>(opcode_table[ADD_SP_R8], "ADD SP R8", 4);
  opcode_table[ADD_SP_R8].execute_ = Add<&Register::a_, k_StackPointer>;
  // We cannot algorithmically set std::functions to opcode table
  BINARY_GB_ALL_REG(BINARY_GB_EXECUTE_EQUALS_OPERATION_REG);
  BINARY_GB_EXECUTE_EQUALS_OPERATION_REG_INDIRECT; 
}
void InitPushAndPop(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset; 
  const std::array<std::string, 8> k_RegisterNames = {"BC", "DE", "HL", "AF"};
  uint8_t register_index = 0;

  // Init Push instructions
  for (uint16_t opcode = 0xC1; opcode <= 0xF1; opcode+= 16) { 
    const std::string k_RegisterName = k_RegisterNames[register_index % 4];
    opcode_table[opcode].opcode_ = PrintOpcode<>;
    opcode_table[opcode].SetMachineCycles(3);
    opcode_table[opcode].mnemonic_ = std::format("POP {}", k_RegisterName); 
    register_index++;
  }

  register_index = 0; 
  // Init Push instructions
  for (uint16_t opcode = 0xC5; opcode <= 0xF5; opcode+= 16) {
    const std::string k_RegisterName = k_RegisterNames[register_index % 4];
    opcode_table[opcode].opcode_ = PrintOpcode<>;
    opcode_table[opcode].SetMachineCycles(3); 
    opcode_table[opcode].mnemonic_ = std::format("PUSH {}", k_RegisterName);
    register_index++; 
  }

  BINARY_GB_REPEAT_FOR_ALL_16BIT_REG(BINARY_GB_EXECUTE_POP_AND_PUSH);
}

void InitIncrementAndDecrement(std::array<Opcode, 512>& opcode_table) {
using namespace binary::gb::instructionset;
  const std::array<std::string, 8> k_16BitRegisterNames = {"BC", "DE", "HL", 
                                                           "SP"}; 
  const std::array<std::string, 8> k_RegisterNames = {"B", "C", "D",    "E",
                                                      "H", "L", "(HL)", "A"};

uint8_t register_index = 0; 
  for (uint8_t opcode = 0x03; opcode <= 0x3D; opcode += 8) { 
    const std::string k_RegisterName = k_RegisterNames[register_index];
    const std::string k_16BitRegisterName = k_16BitRegisterNames[register_index];
    const uint8_t k_16BitOpcode = opcode; 
    const uint8_t k_IncrementOpcode = opcode + 1;
    const uint8_t k_DecrementOpcode = opcode + 2; 
  
    // Indirect instruction on opcode 0x34 and 0x35 takes 3 machine instructions
    const uint8_t cycles = (opcode == 0x34 || opcode == 0x35) ? 1 : 3;
    if ((opcode & 0x03) == 0x03) { 
      opcode_table[k_16BitOpcode].opcode_ = PrintOpcode<>;
      opcode_table[k_16BitOpcode].SetMachineCycles(2); 
      opcode_table[k_16BitOpcode].mnemonic_ = 
        std::format("INC {}", k_16BitRegisterName); 
    } else {
      opcode_table[k_16BitOpcode].opcode_ = PrintOpcode<>;
      opcode_table[k_16BitOpcode].SetMachineCycles(2); 
      opcode_table[k_16BitOpcode].mnemonic_ = 
        std::format("DEC {}", k_16BitRegisterName); 
    }
  
    opcode_table[k_IncrementOpcode].SetMachineCycles(cycles);
    opcode_table[k_IncrementOpcode].opcode_ = PrintOpcode<>;
    opcode_table[k_IncrementOpcode].mnemonic_ = 
      std::format("INC {}", k_RegisterName);
  
    opcode_table[k_DecrementOpcode].SetMachineCycles(cycles);
    opcode_table[k_DecrementOpcode].mnemonic_ =
      std::format("DEC {}", k_RegisterName); 
    opcode_table[k_DecrementOpcode].opcode_ = PrintOpcode<>;
    register_index++;
  }
  opcode_table[DEC__HL].execute_ = Decrement<uint16_t, &Register::hl_, 
                                             k_RegisterIndirect>;
  opcode_table[INC__HL].execute_ = Increment<uint16_t, &Register::hl_,
                                             k_RegisterIndirect>;

  BINARY_GB_ALL_REG(BINARY_GB_EXECUTE_DEC_AND_INC);
  BINARY_GB_EXECUTE_16BIT_DEC_AND_INC_ALL_REG(BINARY_GB_EXECUTE_16BIT_DEC_AND_INC)
}

void NullOpcode(GameBoy* gb) { 
  gb->reg_.program_counter_++; 
}
void InitConditional(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  // There is no pattern in the opcode table, therefore we need to input all the
  // values manually
  constexpr bool k_Branch = true;
  // Return Opcodes
  InitGenericOpcode<1, 5>(opcode_table[RET_NZ], "RET NZ", 2);
  opcode_table[RET_NZ].execute_ = Return<k_Branch, k_BitIndexZ, false>; 
  InitGenericOpcode<1, 5>(opcode_table[RET_NC], "RET NC", 2);
  opcode_table[RET_NC].execute_ = Return<k_Branch, k_BitIndexC, false>;   
  InitGenericOpcode<1, 5>(opcode_table[RET_Z], "RET Z", 2); 
  opcode_table[RET_Z].execute_ = Return<k_Branch, k_BitIndexZ, true>;   
  InitGenericOpcode<1, 5>(opcode_table[RET_C], "RET C", 2); 
  opcode_table[RET_C].execute_ = Return<k_Branch, k_BitIndexC, true>; 
  InitGenericOpcode<1, 4>(opcode_table[RET], "RET", 4);
  opcode_table[RET].execute_ = Return<!k_Branch>;
  InitGenericOpcode<1, 4>(opcode_table[RETI], "RETI", 4);
  opcode_table[RETI].execute_ = ReturnFromInterruptHandler;

  // Jump Relative 
  InitGenericOpcode<2, 3>(opcode_table[JR_NZ_R8], "JR NZ", 2);
  opcode_table[JR_NZ_R8].execute_ = JumpRelative<k_Branch, k_BitIndexZ, false>;
  InitGenericOpcode<2, 3>(opcode_table[JR_NC_R8], "JR NC", 2);
  opcode_table[JR_NC_R8].execute_ = JumpRelative<k_Branch, k_BitIndexC, false>;
  InitGenericOpcode<2, 3>(opcode_table[JR_Z_R8], "JR Z", 2);
  opcode_table[JR_Z_R8].execute_ = JumpRelative<k_Branch, k_BitIndexZ, true>;
  InitGenericOpcode<2, 3>(opcode_table[JR_C_R8], "JR C", 2);
  opcode_table[JR_C_R8].execute_ = JumpRelative<k_Branch, k_BitIndexC, true>;
  InitGenericOpcode<2>(opcode_table[JR_R8], "JR ", 3);
  opcode_table[JR_R8].execute_ = JumpRelative<!k_Branch>;

  // Jump
  InitGenericOpcode<3, 4>(opcode_table[JP_NZ_A16], "JP NZ", 3);
  opcode_table[JP_NZ_A16].execute_ = Jump<k_Branch, k_BitIndexZ, false>;
  InitGenericOpcode<3, 4>(opcode_table[JP_NC_A16], "JP NC", 3);
  opcode_table[JP_NC_A16].execute_ = Jump<k_Branch, k_BitIndexC, false>;
  InitGenericOpcode<3, 4>(opcode_table[JP_Z_A16], "JP Z", 3);
  opcode_table[JP_Z_A16].execute_ = Jump<k_Branch, k_BitIndexZ, true>;
  InitGenericOpcode<3, 4>(opcode_table[JP_C_A16], "JP C", 3);
  opcode_table[JP_C_A16].execute_ = Jump<k_Branch, k_BitIndexC, true>;
  InitGenericOpcode<3>(opcode_table[JP_A16],"JP" , 3);
  opcode_table[JP_A16].execute_ = Jump<!k_Branch>;
  InitGenericOpcode<2>(opcode_table[JP__HL], "JR (HL)", 3);
  opcode_table[JP__HL].execute_ = JumpIndirect;

  // Call
  InitGenericOpcode<3, 6>(opcode_table[CALL_NZ_A16], "CALL NZ", 3); 
  opcode_table[CALL_NZ_A16].execute_ = Call<k_Branch, k_BitIndexZ, false>;
  InitGenericOpcode<3, 6>(opcode_table[CALL_NC_A16], "CALL NC", 3);
  opcode_table[CALL_NC_A16].execute_ = Call<k_Branch, k_BitIndexC, false>;
  InitGenericOpcode<3, 6>(opcode_table[CALL_Z_A16], "CALL Z", 3); 
  opcode_table[CALL_Z_A16].execute_ = Call<k_Branch, k_BitIndexZ, true>;
  InitGenericOpcode<3, 6>(opcode_table[CALL_C_A16], "CALL C", 3); 
  opcode_table[CALL_C_A16].execute_ = Call<k_Branch, k_BitIndexC, true>;
  InitGenericOpcode<3, 6>(opcode_table[CALL_A16], "CALL", 6); 
  opcode_table[CALL_A16].execute_ = Call<!k_Branch>;

}
void InitRestart(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  // There is a patttern, I just don't care anymore.
  InitGenericOpcode<1>(opcode_table[RST_00H], "RST 00H", 4);
  opcode_table[RST_00H].execute_ = Restart<0x00>;
  InitGenericOpcode<1>(opcode_table[RST_10H], "RST 10H", 4);
  opcode_table[RST_10H].execute_ = Restart<0x10>;
  InitGenericOpcode<1>(opcode_table[RST_20H], "RST 20H", 4);
  opcode_table[RST_20H].execute_ = Restart<0x20>;
  InitGenericOpcode<1>(opcode_table[RST_30H], "RST 30H", 4);
  opcode_table[RST_20H].execute_ = Restart<0x30>;
  InitGenericOpcode<1>(opcode_table[RST_08H], "RST 08H", 4);
  opcode_table[RST_08H].execute_ = Restart<0x08>;
  InitGenericOpcode<1>(opcode_table[RST_18H], "RST 18H", 4);
  opcode_table[RST_18H].execute_ = Restart<0x18>;
  InitGenericOpcode<1>(opcode_table[RST_28H], "RST 28H", 4);
  opcode_table[RST_28H].execute_ = Restart<0x28>;
  InitGenericOpcode<1>(opcode_table[RST_38H], "RST 38H", 4);
  opcode_table[RST_38H].execute_ = Restart<0x38>;
}
void InitMiscellaneous(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  InitGenericOpcode<1>(opcode_table[PREFIX_CB], "CB", 1);
  opcode_table[PREFIX_CB].execute_ = PrefixCB;
  InitGenericOpcode<1>(opcode_table[EI], "EI", 1);
  opcode_table[EI].execute_ = EnableInterrput;
  InitGenericOpcode<1>(opcode_table[DI], "DI", 1);
  opcode_table[DI].execute_ = DisableInterrput;
  // Find documentation that goes over how these opecode operate in this system
  InitGenericOpcode<1>(opcode_table[HALT], "HALT", 1);
  InitGenericOpcode<2>(opcode_table[STOP], "STOP", 1);
  InitGenericOpcode<1>(opcode_table[NOP], "NOP", 1);
  opcode_table[NOP].execute_ = NoOperation;

  InitGenericOpcode<1>(opcode_table[DAA], "DAA", 1);
  InitGenericOpcode<1>(opcode_table[SCF], "SCF", 1);
  opcode_table[SCF].execute_ = SetCarryFlag; 
  InitGenericOpcode<1>(opcode_table[CPL], "CPL", 1);
  opcode_table[CPL].execute_ = ComplementAccumulator;
  InitGenericOpcode<1>(opcode_table[CCF], "CCF", 1); 
  opcode_table[CCF].execute_ = ComplementCarryFlag; 
}
void InitNullOpcodes(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  // See, this is why I was using macros for init the opcode table
  InitNullOpcode(opcode_table[NUL_D3], NUL_D3);
  InitNullOpcode(opcode_table[NUL_E3], NUL_E3);
  InitNullOpcode(opcode_table[NUL_E4], NUL_E4);
  InitNullOpcode(opcode_table[NUL_F4], NUL_F4);
  InitNullOpcode(opcode_table[NUL_DB], NUL_DB);
  InitNullOpcode(opcode_table[NUL_EB], NUL_EB);
  InitNullOpcode(opcode_table[NUL_EC], NUL_EC);
  InitNullOpcode(opcode_table[NUL_FC], NUL_FC);
  InitNullOpcode(opcode_table[NUL_DD], NUL_DD);
  InitNullOpcode(opcode_table[NUL_ED], NUL_ED);
  InitNullOpcode(opcode_table[NUL_FD], NUL_FD);
}

void InitNullOpcode(Opcode& opcode, uint32_t code) {
  using namespace binary::gb::instructionset;
  opcode.SetMachineCycles(1);
  opcode.opcode_ = PrintOpcode<>;
  opcode.mnemonic_ = std::format("NULL {:02X}", code);
  opcode.execute_ = NullOpcode;
}

void InitLoadInstructionsTable(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;

  InitGenericOpcode<3>(opcode_table[LD_BC_D16], "LD BC D16", 3);
  opcode_table[LD_BC_D16].execute_ =
      Load<uint16_t, &Register::bc_, &Register::bc_, k_Immediate16>;  
  InitGenericOpcode<3>(opcode_table[LD_DE_D16], "LD DE D16", 3); 
  opcode_table[LD_DE_D16].execute_ = 
      Load<uint16_t, &Register::de_, &Register::de_, k_Immediate16>;   
  InitGenericOpcode<3>(opcode_table[LD_HL_D16], "LD HL D16", 3); 
  opcode_table[LD_HL_D16].execute_ = 
      Load<uint16_t, &Register::hl_, &Register::hl_, k_Immediate16>;   
  InitGenericOpcode<3>(opcode_table[LD_SP_D16], "LD SP D16", 3);
  InitGenericOpcode<3>(opcode_table[LD__A16_SP], "LD SP D16", 5); 
  // I doesn't matter what I put in the parameters (register::bc_)
  // k_StackPointer would ignore x and y and only change the stackpointer 
  // same with k_Address16, it would load data from stackpointer to a memory
  // address.
  opcode_table[LD_SP_D16].execute_ = 
      Load<uint16_t, &Register::bc_, &Register::bc_, k_StackPointer>;
  opcode_table[LD__A16_SP].execute_ =
      Load<uint16_t, &Register::bc_, &Register::bc_, k_Address16>;

  const std::array<std::string, 8> k_RegisterNames = {"B", "C", "D",    "E",
                                                      "H", "L", "(HL)", "A"};
  const std::array<std::string, 8> k_16BitRegisterNames = {"(BC)", "(DE)",
                                                           "(HL+)", "(HL-)"};
  uint8_t register_index = 0;
  uint8_t _16_bit_register_index = 0;
  for (uint8_t opcode = 0x02; opcode <= 0x3E; opcode+= 4) {

    opcode_table[opcode].SetMachineCycles(2);
    if ((opcode & 0x02) == 0x02) {
      opcode_table[opcode].mnemonic_ =
          std::format("LD {}, A", k_16BitRegisterNames[_16_bit_register_index]);
      opcode_table[opcode].opcode_ = PrintOpcode<>;
    } else if (((opcode & 0x06) == 0x06) || ((opcode & 0x0E) == 0x0E)){
      opcode_table[opcode].opcode_ = PrintOpcode<2>;
      opcode_table[opcode].mnemonic_ =
          std::format("LD {}, d8", k_RegisterNames[register_index]);
      register_index++;
    } else if ((opcode & 0x0A) == 0x0A) {
      opcode_table[opcode].mnemonic_ =
          std::format("LD A, {}", k_16BitRegisterNames[_16_bit_register_index]);
      opcode_table[opcode].opcode_ = PrintOpcode<>; 
      _16_bit_register_index++;
    }
  }
  // The only mechine cycle that was over 2 clock cycles
  opcode_table[LD__HL_D8].SetMachineCycles(3);
  register_index = 0;

  for (uint8_t opcode = 0x40; opcode < 0x80; ++opcode) {
    const uint8_t k_OperandIndex = opcode % 8;
    std::string register_name = k_RegisterNames[register_index];
    const std::string k_OperandName = k_RegisterNames[k_OperandIndex];

    if ((opcode & 0xF8) == 0x70) {
      // Skip (HL),(HL) instruction
      if (k_OperandIndex == 6) {
        continue;
      }
      opcode_table[opcode].SetMachineCycles(2);
      register_name = "(HL)";
    } else if ((opcode & 0xC7) == 0x76) {
      // Handle HALT instruction
      opcode_table[opcode].SetMachineCycles(1);
    } else {
      opcode_table[opcode].SetMachineCycles(1);
    }

    opcode_table[opcode].opcode_ = PrintOpcode<>;
    opcode_table[opcode].mnemonic_ =
        std::format("LD {},{}", register_name, k_OperandName);

    if ((opcode & 0x07) == 0x07) {
      ++register_index;
    }
  }

  // We cannot algorithmically set std::functions to opcode table
  BINARY_GB_ALL_REG(BINARY_GB_EXECUTE_EQUALS_LOAD_REGX_FROM_REG);
  BINARY_GB_EXECUTE_EQUALS_LOAD_REGX_FROM_INDIRECT_REG
}
}
