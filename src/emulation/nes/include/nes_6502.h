#include <array>
#include <bitset>
#include <functional>
#include <string>
namespace binary::nes {
  enum Instrcution : uint8_t {
  //Nibble 0
    BRK_IMPL = 0x00, ORA_ZPX = 0x01, KIL_02  = 0x02,
    BPL_REL  = 0x10, ORA_ZPY = 0x11, KIL_12  = 0x12,
    JSR_ABS  = 0x20, AND_ZPX = 0x21, KIL_22  = 0x22,
    BML_REL  = 0x30, AND_ZPY = 0x31, KIL_32  = 0x32,
    RIT_IMPL = 0x40, EOR_ZPX = 0x41, KIL_42  = 0x42,
    BVC_REL  = 0x50, EOR_ZPY = 0x51, KIL_52  = 0x52,
    RIS_IMPL = 0x60, ADC_ZPX = 0x61, KIL_62  = 0x62,
    BVS_REL  = 0x70, ADC_ZPY = 0x71, KIL_72  = 0x72,
    NOP_80   = 0x80, STA_ZPX = 0x81, KIL_82  = 0x82,
    BCC_REL  = 0x90, STA_ZPY = 0x91, KIL_92  = 0x92,
    LDY_IMM  = 0xA0, LDA_ZPX = 0xA1, LDX_IMM = 0xA2,
    BCS_REL  = 0xB0, LDA_ZPY = 0xB1, KIL_B2  = 0xB2,
    CPY_IMM  = 0xC0, CMP_ZPX = 0xC1, NOP_C2  = 0xC2,
    BNE_REL  = 0xD0, CMP_ZPY = 0xD1, KIL_D2  = 0xD2,
    CPX_IMM  = 0xE0, SBC_ZPX = 0xE1, NOP_E2  = 0xE2,
    BEQ_REL  = 0xF0, SBC_ZPY = 0xF1, KIL_F2  = 0xF2,
};
}  // namespace binary::nes