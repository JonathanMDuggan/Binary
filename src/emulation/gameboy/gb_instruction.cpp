#include "include/gb_instruction.h"
#include <format>
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

void NoOperationFunction(GameBoy* gb) { return; }
void NoOperation(GameBoy* gb) { NoOperationFunction(gb); }


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
uint16_t binary::gb::GameBoy::Operand16bit() { 
  const uint16_t k_LowByte = memory_[reg_.program_counter_ + 1];
  const uint16_t k_HighByte = memory_[reg_.program_counter_ + 2];
  const uint16_t k_Operand16bit = (k_HighByte << 8) | k_LowByte;
  return k_Operand16bit;
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
}

void InitPrefixTable(std::array<Opcode, 512>& opcode_table) {
  GameBoy cpu = {};
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
    opcode_table[opcode].opcode_ = std::format("CB {:02X}", opcode);
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
    opcode_table[opcode].opcode_ = std::format("{:02X}", opcode);
    opcode_table[opcode].mnemonic_ =
        std::format("{} {}", k_Mnemonics[mnemonic], k_RegisterNames[opcode % 8]);
    if (((opcode + 1) % 8) == 0) {
      mnemonic++;
    }
  }
  // We cannot algorithmically set std::functions to opcode table
  BINARY_GB_ALL_REG(BINARY_GB_EXECUTE_EQUALS_OPERATION_REG);
  BINARY_GB_EXECUTE_EQUALS_OPERATION_REG_INDIRECT; 
}

void InitLoadInstructionsTable(std::array<Opcode, 512>& opcode_table) {
  using namespace binary::gb::instructionset;
  const std::array<std::string, 8> k_RegisterNames = {"B", "C", "D",    "E",
                                                      "H", "L", "(HL)", "A"};

  uint8_t register_index = 0;

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

    opcode_table[opcode].opcode_ = std::format("{:02X}", opcode);
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
