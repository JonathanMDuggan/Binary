#include <gtest/gtest.h>
#include <memory>
#include "../../../src/emulation/gameboy/include/gb_instruction.h"
#include <gmock/gmock.h>
#include <format>
using ::testing::AtLeast;
namespace binary::gb {
class GameBoyTest : public ::testing::Test {
 protected:
  GameBoy gb_;

  void SetUp() override {
    // Initialize GameBoy or set any pre-conditions here
  }

  void TearDown() override {
    // Clean up after tests if necessary
  }

  void VerifyRegisters(const uint8_t value) {
    EXPECT_EQ(gb_.reg_.b_, value)
        << "Register B should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.d_, value)
        << "Register D should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.h_, value)
        << "Register H should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.l_, value)
        << "Register L should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.c_, value)
        << "Register C should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.e_, value)
        << "Register E should be equal to " << static_cast<uint8_t>(value);
    EXPECT_EQ(gb_.reg_.a_, value)
        << "Register A should be equal to " << static_cast<uint8_t>(value);
  }

  void ClearAndVerifyRegisters() {
    gb_.ClearRegisters();
    EXPECT_EQ(gb_.reg_.b_, 0);
    EXPECT_EQ(gb_.reg_.c_, 0);
    EXPECT_EQ(gb_.reg_.d_, 0);
    EXPECT_EQ(gb_.reg_.e_, 0);
    EXPECT_EQ(gb_.reg_.h_, 0);
    EXPECT_EQ(gb_.reg_.l_, 0);
    EXPECT_EQ(gb_.reg_.a_, 0);
  }

};

TEST_F(GameBoyTest, LoadRegDirectOpcodeTable) {
  using namespace binary::gb::instructionset;
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  InitLoadInstructionsTable(*opcode_table);
  EXPECT_EQ(opcode_table->at(0x40).mnemonic_, "LD B,B");
  EXPECT_EQ(opcode_table->at(0x41).mnemonic_, "LD B,C");
  EXPECT_EQ(opcode_table->at(0x42).mnemonic_, "LD B,D");
  EXPECT_EQ(opcode_table->at(0x43).mnemonic_, "LD B,E");
  EXPECT_EQ(opcode_table->at(0x44).mnemonic_, "LD B,H");
  EXPECT_EQ(opcode_table->at(0x45).mnemonic_, "LD B,L");
  
  for (uint8_t opcode = 0x40; opcode < 0x80; opcode++) {
    if (((~opcode & 0x6) == 0) || ((~opcode & 0xE) == 0)) {
      continue;
    }
    if (opcode > 0x77 || opcode < 0x70) {
      EXPECT_EQ(opcode_table->at(opcode).machine_cycles_, 1)

      << "Load Instruction machine_cycles_should equal 1. Use Opcode "
         "opcode["
      << std::format("0x{:X}", opcode)
      << "].SetMachineCycles(1); to fix this issue. The current opcode's "
         "machine cycles are not set correctly, "
      << "which could lead to incorrect emulation timing during branch "
         "operations. Ensure that the machine cycles "
      << "are set to 1 as per the Game Boy's specification for load "
         "instructions.";

      EXPECT_EQ(opcode_table->at(opcode).machine_cycles_branch_, 1)

      << "Load Instruction machine_cycles_branch should equal 1. Use "
         "Opcode opcode["
      << std::format("0x{:X}", opcode)
      << "].SetMachineCycles(1); to fix this issue. The current opcode's "
         "branch machine cycles are not set correctly,"
      << "which could lead to incorrect emulation timing during branch "
         "operations. Ensure that the branch machine cycles"
      << "are set to 1 as per the Game Boy's specification for load "
         "instructions.";
    }
  }
  const uint8_t k_TestValue = 0x98;
  gb_.reg_.h_ = k_TestValue;
  opcode_table->at(LD_L_H).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.l_, k_TestValue)
      << "Register L should be equal to" << std::format("0x{:X}", 0x98);
  uint16_t hl_value =
      static_cast<uint16_t>((gb_.reg_.h_ << 8) | gb_.reg_.l_);
  EXPECT_EQ(gb_.reg_.hl_, hl_value);
}
TEST_F(GameBoyTest, AddRegXtoRegYTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique <std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);

  // Test Add Mnemonics 
  for (size_t i = 0x80; i < 0x88; i++) {
    if (((~i & 0x6) == 0) || ((~i & 0xE) == 0)) {
      continue;
    }

    EXPECT_EQ(opcode_table->at(i).mnemonic_,
              std::format("ADD {}", k_Letter[i % 8]));
  }
  // Test Adding two registers
  gb_.reg_.a_ = 2;
  gb_.reg_.b_ = 4;
  opcode_table->at(ADD_B).execute_(&gb_);
  uint16_t af_value = static_cast<uint16_t>((gb_.reg_.a_ << 8) |
                                             gb_.reg_.f_.to_ulong());
  EXPECT_EQ(gb_.reg_.a_, 6);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false) << "Zero flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexN], false) << "Negative flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexH], false) << "Half Carry flag wasn 't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], false) << "Carry flag wasn't set";
  EXPECT_EQ(gb_.reg_.af_, af_value);
  gb_.reg_.a_ = 255;
  gb_.reg_.c_ = 255;
  opcode_table->at(ADD_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0xFE);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], true) << "Carry flag wasn't set";

  gb_.reg_.d_ = 2;
  opcode_table->at(ADD_D).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], true) << "Carry flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true) << "Zero flag wasn't set";
}

TEST_F(GameBoyTest, SubRegXTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);

  gb_.reg_.a_ = 6;
  gb_.reg_.b_ = 4;
  opcode_table->at(SUB_B).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 2);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false) << "Zero flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexN], true)  << "Negative flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexH], false) << "Half Carry flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], false) << "Carry flag wasn't set";
  gb_.reg_.a_ = 255;
  gb_.reg_.c_ = 255;
  opcode_table->at(SUB_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true) << "Zero flag wasn't set";
  gb_.reg_.a_ = 10;
  gb_.reg_.d_ = 11;
  opcode_table->at(SUB_D).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0xFF);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], true) << "Carry flag wasn't set";
}

TEST_F(GameBoyTest, OrRegXTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);

  gb_.reg_.a_ = 0b00001111;
  gb_.reg_.b_ = 0b11110000;
  opcode_table->at(OR_B).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0xFF) << "0b00001111 OR 0b11110000 does not equal: "
                               << gb_.reg_.a_;
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false);
  gb_.reg_.a_ = 0b00000000;
  gb_.reg_.c_ = 0b00000000;
  opcode_table->at(OR_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true);
}

TEST_F(GameBoyTest, XorRegXTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);

  gb_.reg_.a_ = 0b11111111;
  gb_.reg_.b_ = 0b11110000;
  opcode_table->at(XOR_B).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0x0F)
      << "0b11111111 XOR 0b11110000 does not equal: " << gb_.reg_.a_;
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false)
      << " Zero flag was set when register a was " << gb_.reg_.a_;
  gb_.reg_.a_ = 0b00000000;
  gb_.reg_.c_ = 0b00000000;
  opcode_table->at(XOR_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true);
}

TEST_F(GameBoyTest, AndRegXTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);
  gb_.reg_.a_ = 0b11111111;
  gb_.reg_.b_ = 0b11110000;
  opcode_table->at(AND_B).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0xF0) << "0b11111111 AND 0b11110000 does not equal: "
                               << std::format("{:8b}", gb_.reg_.a_);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false) 
    << " Zero flag was set when register a was " << gb_.reg_.a_;
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexH], true);
  gb_.reg_.a_ = 0b00000000;
  gb_.reg_.c_ = 0b00000000;
  opcode_table->at(AND_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexH], true);
}

TEST_F(GameBoyTest, CompareRegXTable) {
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  const std::array<std::string, 8> k_Letter = {"B", "C", "D",  "E",
                                               "H", "L", "HL", "A"};
  Init8BitArithmeticLogicRegisterDirectTable(*opcode_table);
  // This is just subtraction instruction with a different context.
  gb_.reg_.a_ = 6;
  gb_.reg_.b_ = 4;
  opcode_table->at(SUB_B).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 2);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], false) << "Zero flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexN], true) << "Negative flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexH], false) << "Half Carry flag wasn't set";
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], false) << "Carry flag wasn't set";
  gb_.reg_.a_ = 255;
  gb_.reg_.c_ = 255;
  opcode_table->at(SUB_C).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexZ], true) << "Zero flag wasn't set";
  gb_.reg_.a_ = 10;
  gb_.reg_.d_ = 11;
  opcode_table->at(SUB_D).execute_(&gb_);
  EXPECT_EQ(gb_.reg_.a_, 0xFF);
  EXPECT_EQ(gb_.reg_.f_[k_BitIndexC], true) << "Carry flag wasn't set";

}
}  // namespace binary

