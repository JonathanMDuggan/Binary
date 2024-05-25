#include <gtest/gtest.h>
#include <memory>
#include "../../../src/emulation/gameboy/include/gb_instruction.h"
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

  void VerifyRegisters(const uint8_t expected) {
    EXPECT_EQ(gb_.reg_.c_, expected);
    EXPECT_EQ(gb_.reg_.b_, expected);
    EXPECT_EQ(gb_.reg_.d_, expected);
    EXPECT_EQ(gb_.reg_.h_, expected);
    EXPECT_EQ(gb_.reg_.l_, expected);
    EXPECT_EQ(gb_.reg_.e_, expected);
    EXPECT_EQ(gb_.reg_.a_, expected);
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
  EXPECT_EQ(gb_.reg_.hl_, 0x0101);
  VerifyRegisters(gb_.reg_.h_);
  ClearAndVerifyRegisters();

  // Test High byte
  gb_.reg_.h_ = 1;
  LoadRegHFromRegH(&gb_);
  EXPECT_EQ(gb_.reg_.hl_, 0x0100);
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
}
}  // namespace binary

