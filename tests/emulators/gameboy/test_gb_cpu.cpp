#include <gtest/gtest.h>
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
}  // namespace binary