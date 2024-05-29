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

TEST_F(GameBoyTest, LoadRegXfromA) {
  using namespace binary::gb::instructionset;
  gb_.reg_.a_ = 1;
  LoadRegBFromRegA(&gb_);
  LoadRegDFromRegA(&gb_);
  LoadRegHFromRegA(&gb_);
  LoadRegLFromRegA(&gb_);
  LoadRegCFromRegA(&gb_);
  LoadRegEFromRegA(&gb_);
  LoadRegAFromRegA(&gb_);
  EXPECT_EQ(gb_.reg_.af_, 0x0100);
  VerifyRegisters(gb_.reg_.a_);
  ClearAndVerifyRegisters();
}

TEST_F(GameBoyTest, LoadRegXfromH) {
  using namespace binary::gb::instructionset;
  gb_.reg_.h_ = 1;
  LoadRegBFromRegH(&gb_);
  LoadRegDFromRegH(&gb_);
  LoadRegHFromRegH(&gb_);
  LoadRegLFromRegH(&gb_);
  LoadRegCFromRegH(&gb_);
  LoadRegEFromRegH(&gb_);
  LoadRegAFromRegH(&gb_);
  EXPECT_EQ(gb_.reg_.hl_, 0x0101)
      << "16-bit register HL must update its value to reflect changes in its "
         "8-bit registers.\n"
      << "Expected HL to be 0x0101 (0x01 in high byte from H and 0x01 in low "
         "byte from L).\n"
      << "Possible issue: The high byte of HL might not be properly updated "
         "from register H.\n"
      << "Ensure HL is set such that its high byte equals H and low byte "
         "equals L.";

  VerifyRegisters(gb_.reg_.h_);
  ClearAndVerifyRegisters();

  // Test High byte
  gb_.reg_.h_ = 1;
  LoadRegHFromRegH(&gb_);
  EXPECT_EQ(gb_.reg_.hl_, 0x0100)
      << "16-bit register HL must update its value to reflect changes in its "
         "8-bit registers.\n"
      << "Expected HL to be 0x0100 (0x01 in high byte from H and 0x00 in low "
         "byte from L).\n"
      << "Possible issue: The high byte of HL might not be properly updated "
         "from register H.\n"
      << "Ensure HL is set such that its high byte equals H and low byte "
         "equals L.";
}

TEST_F(GameBoyTest, LoadRegDirectOpcodeTable) {
  using namespace binary::gb::instructionset;
  std::unique_ptr<std::array<Opcode, 512>> opcode_table;
  opcode_table = std::make_unique<std::array<Opcode, 512>>();
  Init8BitLoadInstructionsTable(*opcode_table);
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
  EXPECT_EQ(gb_.reg_.a_, 6);
}
}  // namespace binary

