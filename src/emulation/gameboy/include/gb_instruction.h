// Purpose: This header file contains the following 
//  * Instruction Set Opcodes Enums
//  * Memory Map
//  * IO Ranges
//  * VRAM memory map
//  * Hardware Registers
//  * CPU Flag Mask
//  * Nintendo Logo
//  * New/Old licensee code
//  * Cartridge Type Flags
//  * ROM size Flags
#include "gb_cpu.h"
namespace retro::gb {
class GameBoy {
public:
  SM83 sm83; 
private:
};
// TDLR: I broke the style guide here
// The Google Style Guide states all new enums should be prefix with k, however
// if I did that the names would be harder to read.

///Gameboy CPU (LR35902) instruction set Documentation:
// Reference: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
// Here are some naming conventions for the enums
//
// NUL: means there is no instruction for the opcode number, it's suffix by the
//      corresponding hexadecimal number
//
// p or m: p means +, and m means -, in C/C++ you cannot put those tokens in the
//         identifier name
//
// The documentation for the Gameboy cpu (LR35902) instruction set includes
// brackets in its name. C/C++ cannot use brackets for its identifier, so
// instead we use _ as the prefix
// 
enum class Instruction {
//Nibble 0            Nibble 1          Nibble 2          Nibble 3
  NOP         = 0x00, LD_BC_D16 = 0x01, LD__BC_A  = 0x02, INC_BC    = 0x03,
  STOP        = 0x10, LD_DE_D16 = 0x11, LD__DE_A  = 0x12, INC_DE    = 0x13,
  JR_NZ_R8    = 0x20, LD_HL_D16 = 0x21, LD__HLp_A = 0x22, INC_HL    = 0x23,
  JR_NC_R8    = 0x30, LD_SP_D16 = 0x31, LD__HLm_A = 0x32, INC_SP    = 0x33,
  LD_B_B      = 0x40, LD_B_C    = 0x41, LD_B_D    = 0x42, LD_B_E    = 0x43,
  LD_D_B      = 0x50, LD_D_C    = 0x51, LD_D_D    = 0x52, LD_D_E    = 0x53,
  LD_H_B      = 0x60, LD_H_C    = 0x61, LD_H_D    = 0x62, LD_H_E    = 0x63,
  LD__HL_B    = 0x70, LD__HL_C  = 0x71, LD__HL_D  = 0x72, LD__HL_E  = 0x73,
  ADD_A_B     = 0x80, ADD_A_C   = 0x81, ADD_A_D   = 0x82, ADD_A_E   = 0x83,
  SUB_B       = 0x90, SUB_C     = 0x91, SUB_D     = 0x92, SUB_E     = 0x93,
  AND_B       = 0xA0, AND_C     = 0xA1, ADD_D     = 0xA2, ADD_E     = 0xA3,
  OR_B        = 0xB0, OR_C      = 0xB1, OR_D      = 0xB2, OR_E      = 0xB3,
  RET_NZ      = 0xC0, POP_BC    = 0xC1, JP_NZ_A16 = 0xC2, JP_A16    = 0xC3,
  RET_NC      = 0xD0, POP_DE    = 0xD1, JP_NC_A16 = 0xD2, NUL_D3    = 0xD3,
  LDH__A8_A   = 0xE0, POP_HL    = 0xE1, LD__C_A   = 0xE2, NUL_E3    = 0xE3,
  LDH_A__A8   = 0xF0, POP_AF    = 0xF1, LC_A__C   = 0xF2, DI        = 0xF3,
//Nibble 4            Nibble 5          Nibble 6          Nibble 7
  INC_B       = 0x04, DEC_B     = 0x05, LD_B_D8   = 0x06, RLCA      = 0x07,
  INC_D       = 0x14, DEC_D     = 0x15, LD_D_D8   = 0x16, RLA       = 0x17,
  INC_H       = 0x24, DEC_H     = 0x25, LD_H_D8   = 0x26, DAA       = 0x27,
  INC__HL     = 0x34, DEC__HL   = 0x35, LD__HL_D8 = 0x36, SCF       = 0x37,
  LD_B_H      = 0x44, LD_B_L    = 0x45, LD_B__HL  = 0x46, LD_B_A    = 0x47,
  LD_D_H      = 0x54, LD_D_L    = 0x55, LD_D__HL  = 0x56, LD_D_A    = 0x57,
  LD_H_H      = 0x64, LD_H_L    = 0x65, LD_H__HL  = 0x66, LD_H_A    = 0x67,
  LD__HL_H    = 0x74, LD__HL_L  = 0x75, HALT      = 0x76, LD__HL_A  = 0x77,
  ADD_A_H     = 0x84, ADD_A_L   = 0x85, ADD_A__HL = 0x86, ADD_A_A   = 0x87,
  SUB_H       = 0x94, SUB_L     = 0x95, SUB__HL   = 0x96, SUB_A     = 0x97,
  AND_H       = 0xA4, AND_L     = 0xA5, AND__HL   = 0xA6, AND_A     = 0xA7,
  OR_H        = 0xB4, OR_L      = 0xB5, OR__HL    = 0xB6, OR_A      = 0xB7,
  CALL_NZ_A16 = 0xC4, PUSH_BC   = 0xC5, ADD_A_D8  = 0xC6, RST_00H   = 0xC7,
  CALL_NC_A16 = 0xD4, PUSH_DE   = 0xD5, SUB_D8    = 0xD6, RST_10H   = 0xD7,
  NUL_E4      = 0xE4, PUSH_HL   = 0xE5, AND_D8    = 0xE6, RST_20H   = 0xE7,
  NUL_F4      = 0xF4, PUSH_AF   = 0xF5, OR_D8     = 0xF6, RST_30H   = 0xF7,
//Nibble 8            Nibble 9          Nibble A          Nibble B
  LD__A16_SP  = 0x08, ADD_HL_BC = 0x09, LD__A_BC  = 0x0A, DEC_DC    = 0x0B,
  JR_R8       = 0x18, ADD_HL_DE = 0x19, LD__A_DE  = 0x1A, DEC_DE    = 0x1B,
  JR_Z_R8     = 0x28, ADD_HL_HL = 0x29, LD__A_HLp = 0x2A, DEC_HL    = 0x2B,
  JR_C_R8     = 0x38, ADD_HL_SP = 0x39, LD__A_HLm = 0x3A, DEC_SP    = 0x3B,
  LD_C_B      = 0x48, LD_C_C    = 0x49, LD_C_D    = 0x4A, LD_C_E    = 0x4B,
  LD_E_B      = 0x58, LD_E_C    = 0x59, LD_E_D    = 0x5A, LD_E_E    = 0x5B,
  LD_L_B      = 0x68, LD_L_C    = 0x69, LD_L_D    = 0x6A, LD_L_E    = 0x6B,
  LD_A_B      = 0x78, LD_A_C    = 0x79, LD_A_D    = 0x7A, LD_A_E    = 0x7B,
  ADC_A_B     = 0x88, ADC_A_C   = 0x89, ADC_A_D   = 0x8A, ADC_A_E   = 0x8B,
  SBC_A_B     = 0x98, SBC_A_C   = 0x99, SBC_A_D   = 0x9A, SBC_A_E   = 0x9B,
  XOR_B       = 0xA8, XOR_C     = 0xA9, XOR_D     = 0xAA, XOR_E     = 0xAB,
  CP_B        = 0xB8, CP_C      = 0xB9, CP_D      = 0xBA, CP_E      = 0xBB,
  RET_Z       = 0xC8, RET       = 0xC9, JP_Z_A16  = 0xCA, PREFIX_CB = 0xCB,
  RET_C       = 0xD8, RETI      = 0xD9, JP_C_A16  = 0xDA, NUL_DB    = 0xDB,
  ADD_SP_R8   = 0xE8, JP__HL    = 0xE9, LD__A16_A = 0xEA, NUL_EB    = 0xEB,
  LD_HL_SPpR8 = 0xF8, LD_SP_HL  = 0xF9, LD__A_A16 = 0xFA, EI        = 0xFB,
//Nibble C            Nibble D          Nibble E          Nibble F
  INC_C       = 0x0C, DEC_C     = 0x0D, LD_C_D8   = 0x0E, RRCA      = 0x0F,
  INC_E       = 0x1C, DEC_E     = 0x1D, LD_E_D8   = 0x1E, RRA       = 0x1F,
  INC_L       = 0x2C, DEC_L     = 0x2D, LD_L_D8   = 0x2E, CPL       = 0x2F,
  INC_A       = 0x3C, DEC_A     = 0x3D, LD_A_D8   = 0x3E, CCF       = 0x3F,
  LD_C_H      = 0x4C, LD_C_L    = 0x4D, LD_C__HL  = 0x4E, LD_C_A    = 0x4F,
  LD_E_H      = 0x5C, LD_E_L    = 0x5D, LD_E__HL  = 0x5E, LD_E_A    = 0x5F,
  LD_L_H      = 0x6C, LD_L_L    = 0x6D, LD_L__HL  = 0x6E, LD_L_A    = 0x6F,
  LD_A_H      = 0x7C, LD_A_L    = 0x7D, LD_A__HL  = 0x7E, LD_A_A    = 0x7F,
  ADC_A_H     = 0x8C, ADC_A_L   = 0x8D, ADC_A__HL = 0x8E, ADC_A_A   = 0x8F,
  SBC_A_H     = 0x9C, SBC_A_L   = 0x9D, SBC_A__HL = 0x9E, SBC_A_A   = 0x9F,
  XOR_H       = 0xAC, XOR_L     = 0xAD, XOR__HL   = 0xAE, XOR_A     = 0xAF,
  CP_H        = 0xBC, CP_L      = 0xBD, CP__HL    = 0xBE, CP_A      = 0xBF,
  CALL_Z_A16  = 0xCC, CALL_A16  = 0xCD, ADC_A_D8  = 0xCE, RST_08H   = 0xCF,
  CALL_C_A16  = 0xDC, NUL_DD    = 0xDD, SBC_A_D8  = 0xDE, RST_18H   = 0xDF,
  NUL_EC      = 0xEC, NUL_ED    = 0xED, XOR_D8    = 0xEE, RST_28H   = 0xEF,
  NUL_FC      = 0xFC, NUL_FD    = 0xFD, CP_D8     = 0xFE, RST_38H   = 0xFF,
  
// PREFIX_CB Opcodes
// 
//Nibble 0            Nibble 1          Nibble 2          Nibble 3
  RLC_B       = 0x00, RLC_C     = 0x01, RLC_D     = 0x02, RLC_E     = 0x03,
  RL_B        = 0x10, RL_C      = 0x11, RL_D      = 0x12, RL_E      = 0x13,
  SLA_B       = 0x20, SLA_C     = 0x21, SLA_D     = 0x22, SLA_E     = 0x23,
  SWAP_B      = 0x30, SWAP_C    = 0x31, SWAP_D    = 0x32, SWAP_E    = 0x33,
  BIT_0_B     = 0x40, BIT_0_C   = 0x41, BIT_0_D   = 0x42, BIT_0_E   = 0x43,
  BIT_2_B     = 0x50, BIT_2_C   = 0x51, BIT_2_D   = 0x52, BIT_2_E   = 0x53,
  BIT_4_B     = 0x60, BIT_4_C   = 0x61, BIT_4_D   = 0x62, BIT_4_E   = 0x63,
  BIT_6_B     = 0x70, BIT_6_C   = 0x71, BIT_6_D   = 0x72, BIT_6_E   = 0x73,
  RES_0_B     = 0x80, RES_0_C   = 0x81, RES_0_D   = 0x82, RES_0_E   = 0x83,
  RES_2_B     = 0x90, RES_2_C   = 0x91, RES_2_D   = 0x92, RES_2_E   = 0x93,
  RES_4_B     = 0xA0, RES_4_C   = 0xA1, RES_4_D   = 0xA2, RES_4_E   = 0xA3,
  RES_6_B     = 0xB0, RES_6_C   = 0xB1, RES_6_D   = 0xB2, RES_6_E   = 0xB3,
  SET_0_B     = 0xC0, SET_0_C   = 0xC1, SET_0_D   = 0xC2, SET_0_E   = 0xC3,
  SET_2_B     = 0xD0, SET_2_C   = 0xD1, SET_2_D   = 0xD2, SET_2_E   = 0xD3,
  SET_4_B     = 0xE0, SET_4_C   = 0xE1, SET_4_D   = 0xE2, SET_4_E   = 0xE3,
  SET_6_B     = 0xF0, SET_6_C   = 0xF1, SET_6_D   = 0xF2, SET_6_E   = 0xF3,
//Nibble 4            Nibble 5          Nibble 6          Nibble 7
  RLC_H       = 0x04, RLC_L     = 0x05, RLC__HL   = 0x06, RLC_A     = 0x07,
  RL_H        = 0x14, RL_L      = 0x15, RL__HL    = 0x16, RL_A      = 0x17,
  SLA_H       = 0x24, SLA_L     = 0x25, SLA__HL   = 0x26, SLA_A     = 0x27,
  SWAP_H      = 0x34, SWAP_L    = 0x35, SWAP__HL  = 0x36, SWAP_A    = 0x37,
  BIT_0_H     = 0x44, BIT_0_L   = 0x45, BIT_0__HL = 0x46, BIT_0_A   = 0x47,
  BIT_2_H     = 0x54, BIT_2_L   = 0x55, BIT_2__HL = 0x56, BIT_2_A   = 0x57,
  BIT_4_H     = 0x64, BIT_4_L   = 0x65, BIT_4__HL = 0x66, BIT_4_A   = 0x67,
  BIT_6_H     = 0x74, BIT_6_L   = 0x75, BIT_6__HL = 0x76, BIT_6_A   = 0x77,
  RES_0_H     = 0x84, RES_0_L   = 0x85, RES_0__HL = 0x86, RES_0_A   = 0x87,
  RES_2_H     = 0x94, RES_2_L   = 0x95, RES_2__HL = 0x96, RES_2_A   = 0x97,
  RES_4_H     = 0xA4, RES_4_L   = 0xA5, RES_4__HL = 0xA6, RES_4_A   = 0xA7,
  RES_6_H     = 0xB4, RES_6_L   = 0xB5, RES_6__HL = 0xB6, RES_6_A   = 0xB7,
  SET_0_H     = 0xC4, SET_0_L   = 0xC5, SET_0__HL = 0xC6, SET_0_A   = 0xC7,
  SET_2_H     = 0xD4, SET_2_L   = 0xD5, SET_2__HL = 0xD6, SET_2_A   = 0xD7,
  SET_4_H     = 0xE4, SET_4_L   = 0xE5, SET_4__HL = 0xE6, SET_4_A   = 0xE7,
  SET_6_H     = 0xF4, SET_6_L   = 0xF5, SET_6__HL = 0xF6, SET_6_A   = 0xF7,
//Nibble 8            Nibble 9          Nibble A          Nibble B
  RRC_B       = 0x08, RRC_C     = 0x09, RRC_D     = 0x0A, RRC_E     = 0x0B,
  RR_B        = 0x18, RR_C      = 0x19, RR_D      = 0x1A, RR_E      = 0x1B,
  SRA_B       = 0x28, SRA_C     = 0x29, SRA_D     = 0x2A, SRA_E     = 0x2B,
  SRL_B       = 0x38, SRL_C     = 0x39, SRL_D     = 0x3A, SRL_E     = 0x3B,
  BIT_1_B     = 0x48, BIT_1_C   = 0x49, BIT_1_D   = 0x4A, BIT_1_E   = 0x4B,
  BIT_3_B     = 0x58, BIT_3_C   = 0x59, BIT_3_D   = 0x5A, BIT_3_E   = 0x5B,
  BIT_5_B     = 0x68, BIT_5_C   = 0x69, BIT_5_D   = 0x6A, BIT_5_E   = 0x6B,
  BIT_7_B     = 0x78, BIT_7_C   = 0x79, BIT_7_D   = 0x7A, BIT_7_E   = 0x7B,
  RES_1_B     = 0x88, RES_1_C   = 0x89, RES_1_D   = 0x8A, RES_1_E   = 0x8B,
  RES_3_B     = 0x98, RES_3_C   = 0x99, RES_3_D   = 0x9A, RES_3_E   = 0x9B,
  RES_5_B     = 0xA8, RES_5_C   = 0xA9, RES_5_D   = 0xAA, RES_5_E   = 0xAB,
  RES_7_B     = 0xB8, RES_7_C   = 0xB9, RES_7_D   = 0xBA, RES_7_E   = 0xBB,
  SET_1_B     = 0xC8, SET_1_C   = 0xC9, SET_1_D   = 0xCA, SET_1_E   = 0xCB,
  SET_3_B     = 0xD8, SET_3_C   = 0xD9, SET_3_D   = 0xDA, SET_3_E   = 0xDB,
  SET_5_B     = 0xE8, SET_5_C   = 0xE9, SET_5_D   = 0xEA, SET_5_E   = 0xEB,
  SET_7_B     = 0xF8, SET_7_C   = 0xF9, SET_7_D   = 0xFA, SET_7_E   = 0xFB,
//Nibble C            Nibble D          Nibble E          Nibble F
  RRC_H       = 0x0C, RRC_L     = 0x0D, RRC__HL   = 0x0E, RRC_A     = 0x0F,
  RR_H        = 0x1C, RR_L      = 0x1D, RR__HL    = 0x1E, RR_A      = 0x1F,
  SRA_H       = 0x2C, SRA_L     = 0x2D, SRA__HL   = 0x2E, SRA_A     = 0x2F,
  SRL_H       = 0x3C, SRL_L     = 0x3D, SRL__HL   = 0x3E, SRL_A     = 0x3F,
  BIT_1_H     = 0x4C, BIT_1_L   = 0x4D, BIT_1__HL = 0x4E, BIT_1_A   = 0x4F,
  BIT_3_H     = 0x5C, BIT_3_L   = 0x5D, BIT_3__HL = 0x5E, BIT_3_A   = 0x5F,
  BIT_5_H     = 0x6C, BIT_5_L   = 0x6D, BIT_5__HL = 0x6E, BIT_5_A   = 0x6F,
  BIT_7_H     = 0x7C, BIT_7_L   = 0x7D, BIT_7__HL = 0x7E, BIT_7_A   = 0x7F,
  RES_1_H     = 0x8C, RES_1_L   = 0x8D, RES_1__HL = 0x8E, RES_1_A   = 0x8F,
  RES_3_H     = 0x9C, RES_3_L   = 0x9D, RES_3__HL = 0x9E, RES_3_A   = 0x9F,
  RES_5_H     = 0xAC, RES_5_L   = 0xAD, RES_5__HL = 0xAE, RES_5_A   = 0xAF,
  RES_7_H     = 0xBC, RES_7_L   = 0xBD, RES_7__HL = 0xBE, RES_7_A   = 0xBF,
  SET_1_H     = 0xCC, SET_1_L   = 0xCD, SET_1__HL = 0xCE, SET_1_A   = 0xCF,
  SET_3_H     = 0xDC, SET_3_L   = 0xDD, SET_3__HL = 0xDE, SET_3_A   = 0xDF,
  SET_5_H     = 0xEC, SET_5_L   = 0xED, SET_5__HL = 0xEE, SET_5_A   = 0xEF,
  SET_7_H     = 0xFC, SET_7_L   = 0xFD, SET_7__HL = 0xFE, SET_7_A   = 0xFF
};
// Thank God for gbdev.io, documentation can be found here:
// https://gbdev.io/pandocs/Memory_Map.html
enum class MemoryMap{
  ROM_BANK_00_START    = 0x0000, ROM_BANK_00_END    = 0x3FFF,
  ROM_BANK_01_NN_START = 0x4000, ROM_BANK_01_NN_END = 0x7FFF,
  VIDEO_RAM_START      = 0x8000, VIDEO_RAM_END      = 0x9FFF,
  EXTERNAL_RAM_START   = 0xA000, EXTERNAL_RAM_END   = 0xBFFF,
  WORK_RAM_START       = 0xC000, WORK_RAM_END       = 0xCFFF,
  ECHO_RAM_START       = 0xE000, ECHO_RAM_END       = 0xFDFF,
  OAM_START            = 0xFE00, OAM_END            = 0xFE9F,
  NOT_USABLE_START     = 0xFEA0, NOT_USABLE_END     = 0xFEFF,
  IO_REGISTERS_START   = 0xFF00, IO_REGISTERS_END   = 0xFF7F,
  HIGH_RAM_START       = 0xFF80, HIGH_RAM_END       = 0xFFFE,
  INTERRUPT_ENABLE     = 0xFFFF
};
// Thank God for gbdev.io, documentation can be found here:
// https://gbdev.io/pandocs/Memory_Map.html
enum class IORanges {
  JOYPAD_INPUT          = 0xFF00,
  SERIAL_TRANSFER_START = 0xFF01, SERIAL_TRANSFER_END = 0xFF02,
  TIMER_DIVIDER_START   = 0xFF04, TIMER_DIVIDER_END   = 0xFF07,
  AUDIO_START           = 0xFF10, AUDIO_END           = 0xFF26,
  WAVE_PATTERN_START    = 0xFF30, WAVE_PATTERN_END    = 0xFF3F,
  VRAM_BANK_SELECT      = 0xFF4F,
  DISABLE_BOOT_ROM      = 0xFF50, 
  VRAM_DMA_START        = 0xFF51, VRAM_DMA_END        = 0xFF55,
  BG_OBJ_PALETTES_START = 0xFF68, BG_OBJ_PALETTES_END = 0xFF6B,
  WRAM_BANK_SELECT      = 0xFF70
};
// Thank God for gbdev.io, documentation can be found here:
// https://gbdev.io/pandocs/Hardware_Reg_List.html
enum class HardwareRegisersName {
  P1_JOYP = 0xFF00, // JoyPad
  SB      = 0xFF01, // Serial Transfer Data
  SC      = 0xFF02, // Serial Transfer Control
  DIV     = 0xFF04, // Divider Register
  TIMA    = 0xFF05, // Timer Counter
  TMA     = 0xFF06, // Timer Modulo
  TAC     = 0xFF06, // Timer Control
  IF      = 0xFF0F, // Interrupt Flag
  NR10    = 0xFF10, // Sound Channel 1 Sweep
  NR11    = 0xFF11, // Sound Channel 1 length timer & duty cycle
  NR12    = 0xFF12, // Sound Channel 1 volume & envelope
  NR13    = 0xFF13, // Sound Channel 1 period low
  NR14    = 0xFF14, // Sound Channel 1 period high & control
  NR21    = 0xFF16, // Sound Channel 2 length timer & duty cycle
};

enum CpuFlags {
  Z = 0b10000000,
  N = 0b01000000,
  H = 0b00100000,
  C = 0b00010000
};
typedef struct Opcode {
  Instruction hexadecimal;
  std::string opcode;
  std::string mnemonic;
  std::function<void()> Execute(Device* gb); 

}Opcode;
extern const std::array<Opcode, 512> k_OpcodeLookupTable;
}  // namespace retro::gb

namespace retro::gb::instructionset{
///Function implementation for the GameBoy CPU instruction set 
// Reference: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
///Naming conventions
// * Reg:         Register
// * Immediate8:  d8 or immediate 8 bit data
// * Immediate16: d16 or immediate 16 bit data
// * RegSP:       Stack pointer
///Notes on order of operations:
// The Register that is first mention is the location where the data is stored
// Examples: 
// * LoadRegDFromRegB is equivalent to: D = B
// * AddRegAFromRegD is equivalent to:  A += D
//   
// Nibble 0
extern void (*NoOperation)(GameBoy*);                        // 0x00 NOP
extern void (*Stop)(GameBoy*);                               // 0x10 STOP
extern void (*JumpRelativeSignedIfNotZero)(GameBoy*);        // 0x20 JR NZ R8
extern void (*JumpRelativeSignedIfNotCarry)(GameBoy*);       // 0x30 JR NC R8
extern void (*LoadRegBFromRegB)(GameBoy*);                   // 0x40 LD B,B
extern void (*LoadRegDFromRegB)(GameBoy*);                   // 0x50 LD D,B
extern void (*LoadRegHFromRegB)(GameBoy*);                   // 0x60 LD H,B
extern void (*LoadRegIndirectHLFromRegB)(GameBoy*);          // 0x70 LD (HL),B
extern void (*AddRegAFromB)(GameBoy*);                       // 0x80 ADD A,B
extern void (*SubRegB)(GameBoy*);                            // 0x90 SUB B
extern void (*AndRegB)(GameBoy*);                            // 0xA0 AND B
extern void (*OrRegB)(GameBoy*);                             // 0xB0 OR B
extern void (*ReturnIfNotZero)(GameBoy*);                    // 0xC0 RET NZ
extern void (*ReturnIfNotCarry)(GameBoy*);                   // 0xD0 RET NC
extern void (*LoadHighAddressIntoRegA)(GameBoy*);            // 0xE0 LDH (A8),A
extern void (*LoadRegAIntoHighAddress)(GameBoy*);            // 0xF0 LDH A,(A8)

// Nibble 1
extern void (*LoadRegBCImmediate16)(GameBoy*);               // 0x01 LD BC,d16
extern void (*LoadRegDEImmediate16)(GameBoy*);               // 0x11 LD DE,d16
extern void (*LoadRegHLImmediate16)(GameBoy*);               // 0x21 LD HL,d16
extern void (*LoadStackPointerImmediate16)(GameBoy*);        // 0x31 LD SP,d16
extern void (*LoadRegBFromRegC)(GameBoy*);                   // 0x41 LD B,C
extern void (*LoadRegDFromRegC)(GameBoy*);                   // 0x51 LD D,C
extern void (*LoadRegHFromRegC)(GameBoy*);                   // 0x61 LD H,C
extern void (*LoadIndirectRegHLFromRegC)(GameBoy*);          // 0x71 LD (HL),C
extern void (*AddRegAFromC)(GameBoy*);                       // 0x81 ADD A,C
extern void (*SubRegC)(GameBoy*);                            // 0x91 SUB C
extern void (*AndRegC)(GameBoy*);                            // 0xA1 AND C
extern void (*OrRegC)(GameBoy*);                             // 0xB1 OR C
extern void (*PopRegBC)(GameBoy*);                           // 0xC1 POP BC
extern void (*PopRegDE)(GameBoy*);                           // 0xD1 POP DE
extern void (*PopRegHL)(GameBoy*);                           // 0xE1 POP HL
extern void (*PopRegAF)(GameBoy*);                           // 0xF1 POP AF

// Nibble 2
extern void (*LoadIndirectRegBCFromRegA)(GameBoy*);          // 0x02 LD (BC),A 
extern void (*LoadIndirectRegDEFromRegA)(GameBoy*);          // 0x12 LD (DE),A
extern void (*LoadIndirectIncrementRegHLFromRegA)(GameBoy*); // 0x22 LD (HL+),A
extern void (*LoadIndirectDecrementRegHLFromRegA)(GameBoy*); // 0x32 LD (HL-),A
extern void (*LoadRegBFromRegD)(GameBoy*);                   // 0x42 LD B,D
extern void (*LoadRegDFromRegD)(GameBoy*);                   // 0x52 LD D,D
extern void (*LoadRegHFromRegD)(GameBoy*);                   // 0x62 LD H,D
extern void (*LoadIndirectRegHLFromRegD)(GameBoy*);          // 0x72 LD (HL),D
extern void (*AddRegAFromRegD)(GameBoy*);                    // 0x82 ADD A,D
extern void (*SubRegD)(GameBoy*);                            // 0x92 SUB D
extern void (*AndRegD)(GameBoy*);                            // 0xA2 AND D
extern void (*OrRegD)(GameBoy*);                             // 0xB2 OR D
extern void (*JumpToAddress16IfNotZero)(GameBoy*);           // 0xC2 RET NZ
extern void (*JumpToAddress16IfNotCarry)(GameBoy*);          // 0xD2 RET NC
extern void (*LoadIndirectRegCFromRegA)(GameBoy*);           // 0xE2 LD (C),A
extern void (*LoadRegAFromIndirectC)(GameBoy*);              // 0xF2 LD A,(C)

// Nibble 3 
extern void (*IncrementRegBC)(GameBoy*);                     // 0x03 INC BC
extern void (*IncrementRegDE)(GameBoy*);                     // 0x13 INC DE
extern void (*IncrementRegHL)(GameBoy*);                     // 0x23 INC HL
extern void (*IncrementStackPointer)(GameBoy*);              // 0x33 INC SP
extern void (*LoadRegBFromRegE)(GameBoy*);                   // 0x43 LD B,E
extern void (*LoadRegDFromRegE)(GameBoy*);                   // 0x53 LD D,E
extern void (*LoadRegHFromRegE)(GameBoy*);                   // 0x63 LD H,E
extern void (*LoadIndirectRegHLFromRegE)(GameBoy*);          // 0x73 LD (HL),E
extern void (*AddRegAFromRegE)(GameBoy*);                    // 0x83 ADD A,E
extern void (*SubRegE)(GameBoy*);                            // 0x93 SUB E
extern void (*AndRegE)(GameBoy*);                            // 0xA3 AND E
extern void (*OrRegE)(GameBoy*);                             // 0xB3 OR E
extern void (*JumpToAddress16)(GameBoy*);                    // 0xC3 JP a16
extern void (*NoInstructionD3)(GameBoy*);                    // 0xD3 NUL
extern void (*NoInstructionE3)(GameBoy*);                    // 0xE3 NUL
extern void (*DisableInterrupts)(GameBoy*);                  // 0xF3 DI
                                                             
// Nibble 4                                                  
extern void (*IncrementRegB)(GameBoy*);                      // 0x04 INC B
extern void (*IncrementRegD)(GameBoy*);                      // 0x14 INC D
extern void (*IncrementRegH)(GameBoy*);                      // 0x24 INC H
extern void (*IncrementIndirectRegHL)(GameBoy*);             // 0x34 INC (HL)
extern void (*LoadRegBFromRegH)(GameBoy*);                   // 0x44 LD B, H
extern void (*LoadRegDFromRegH)(GameBoy*);                   // 0x54 LD D, H
extern void (*LoadRegHFromRegH)(GameBoy*);                   // 0x64 LD H, H
extern void (*LoadIndirectRegHLFromRegH)(GameBoy*);          // 0x74 LD (HL),H
extern void (*AddRegARegH)(GameBoy*);                        // 0x84 ADD A,H
extern void (*SubRegH)(GameBoy*);                            // 0x94 SUB H 
extern void (*AndRegH)(GameBoy*);                            // 0xA4 AND H
extern void (*OrRegH)(GameBoy*);                             // 0xB4 OR H
extern void (*CallAddress16IfNotZero)(GameBoy*);             // 0xC4 CALL NZ,a16
extern void (*CallAddress16IfNotCarry)(GameBoy*);            // 0xD4 CALL NC,a16
extern void (*NoInstructionE4)(GameBoy*);                    // 0xE4 NUL
extern void (*NoInstructionF4)(GameBoy*);                    // 0xF4 NUL
                                           
// Nibble 5                                                  
extern void (*DecrementRegB)(GameBoy*);                      // 0x05 DEC B
extern void (*DecrementRegD)(GameBoy*);                      // 0x15 DEC D
extern void (*DecrementRegH)(GameBoy*);                      // 0x25 DEC H
extern void (*DecrementIndirectRegHL)(GameBoy*);             // 0x35 DEC (HL)
extern void (*LoadRegBFromRegL)(GameBoy*);                   // 0x45 LD B, L
extern void (*LoadRegDFromRegL)(GameBoy*);                   // 0x55 LD D, L
extern void (*LoadRegHFromRegL)(GameBoy*);                   // 0x65 LD H, L
extern void (*LoadIndirectRegHFromRegL)(GameBoy*);           // 0x75 LD (HL),L
extern void (*AddRegAFromRegL)(GameBoy*);                    // 0x85 ADD A,L
extern void (*SubRegL)(GameBoy*);                            // 0x95 SUB L
extern void (*AndRegL)(GameBoy*);                            // 0xA5 AND L
extern void (*OrRegL)(GameBoy*);                             // 0xB5 OR L
extern void (*PushRegBC)(GameBoy*);                          // 0xC5 PUSH BC
extern void (*PushRegDE)(GameBoy*);                          // 0xD5 PUSH DE
extern void (*PushRegHL)(GameBoy*);                          // 0xE5 PUSH HL
extern void (*PushRegAF)(GameBoy*);                          // 0xF5 PUSH AF

// Nibble 6                                                  
extern void (*LoadRegBImmediate8)(GameBoy*);                 // 0x06 LD B,d8
extern void (*LoadRegDImmediate8)(GameBoy*);                 // 0x16 LD D,d8
extern void (*LoadRegHImmediate8)(GameBoy*);                 // 0x26 LD H,d8
extern void (*LoadIndirectRegHLImmediate8)(GameBoy*);        // 0x36 LD (HL),d8
extern void (*LoadRegB)(GameBoy*);                           // 0x46 LD B,(HL)
extern void (*LoadRegD)(GameBoy*);                           // 0x56 LD D,(HL)
extern void (*LoadRegH)(GameBoy*);                           // 0x66 LD H,(HL)
extern void (*Halt)(GameBoy*);                               // 0x76 HALT
extern void (*AddRegAFromIndirectRegHL)(GameBoy*);           // 0x86 ADD A,(HL)
extern void (*SubIndirectRegHL)(GameBoy*);                   // 0x96 SUB (HL)
extern void (*AndIndirectRegHL)(GameBoy*);                   // 0xA6 AND (HL)
extern void (*OrIndirectRegHL)(GameBoy*);                    // 0xB6 OR (HL)
extern void (*AddRegAFromImmediate8)(GameBoy*);              // 0xC6 ADD A,d8
extern void (*SubImmediate8)(GameBoy*);                      // 0xD6 SUB d8
extern void (*AndImmediate8)(GameBoy*);                      // 0xE6 AND d8
extern void (*OrImmediate8)(GameBoy*);                       // 0xF6 OR d8
                                                             
// Nibble 7                                                  
extern void (*RotateLeftCarryRegA)(GameBoy*);                // 0x07 RLCA
extern void (*RotateLeftRegA)(GameBoy*);                     // 0x17 RLA
extern void (*DecimalAdjectAccumulator)(GameBoy*);           // 0x27 DAA
extern void (*SetCarryFlag)(GameBoy*);                       // 0x37 SCF
extern void (*LoadRegBFromRegA)(GameBoy*);                   // 0x57 LD B,A
extern void (*LoadRegDFromRegA)(GameBoy*);                   // 0x67 LD D,A
extern void (*LoadRegHFromRegA)(GameBoy*);                   // 0x77 LD H,A
extern void (*LoadIndirectHLFromRegA)(GameBoy*);             // 0x47 LD (HL),A
extern void (*AddRegAFromRegA)(GameBoy*);                    // 0x87 ADD A,A
extern void (*SubRegA)(GameBoy*);                            // 0x97 SUB A
extern void (*AndRegA)(GameBoy*);                            // 0xA7 AND A
extern void (*OrRegA)(GameBoy*);                             // 0xB7 OR A
extern void (*RestartAtAddress00)(GameBoy*);                 // 0xC7 RST 00H
extern void (*RestartAtAddress10)(GameBoy*);                 // 0xD7 RST 10H
extern void (*RestartAtAddress20)(GameBoy*);                 // 0xE7 RST 20H
extern void (*RestartAtAddress30)(GameBoy*);                 // 0xF7 RST 30H
                                                             
// Nibble 8                                                  
extern void (*LoadIndirect16FromStackPointer)(GameBoy*);     // 0x08 LD (a16),SP
extern void (*JumpRelativeSigned8)(GameBoy*);                // 0x18 JR r8
extern void (*JumpRelativeSigned8IfZero)(GameBoy*);          // 0x28 JR Z,r8
extern void (*JumpRelativeSigned8IfCarry)(GameBoy*);         // 0x38 JR C.r8
extern void (*LoadRegCFromRegB)(GameBoy*);                   // 0x48 LD C,B
extern void (*LoadRegEFromRegB)(GameBoy*);                   // 0x58 LD E,B
extern void (*LoadRegLFromRegB)(GameBoy*);                   // 0x68 LD L,B
extern void (*LoadRegAFromRegB)(GameBoy*);                   // 0x78 LD A,B
extern void (*AddWithCarryRegAFromRegB)(GameBoy*);           // 0x88 ADC A,B
extern void (*SubWithCarryRegAFromRegB)(GameBoy*);           // 0x98 SBC A,B
extern void (*XorRegB)(GameBoy*);                            // 0xA8 XOR B
extern void (*CompareRegB)(GameBoy*);                        // 0xB8 CP B
extern void (*ReturnIfZero)(GameBoy*);                       // 0xC8 RET Z
extern void (*ReturnIfCarry)(GameBoy*);                      // 0xD8 RET C
extern void (*AddStackPointerFromSigned8)(GameBoy*);         // 0xE8 ADD SP,r8
extern void (*LoadRegHLFromSigned8PlusRegSP)(GameBoy*);      // 0xF8 LD HL,SP+r8
                                                             
// Nibble 9                                                  
extern void (*AddRegHLFromRegBC)(GameBoy*);                  // 0x09 ADD HL.BC
extern void (*AddRegHLFromRegDE)(GameBoy*);                  // 0x19 ADD HL,DE
extern void (*AddRegHLFromRegHL)(GameBoy*);                  // 0x29 ADD HL,HL
extern void (*AddRegHLFromRegSP)(GameBoy*);                  // 0x39 ADD HL,SP
extern void (*LoadRegCFromRegC)(GameBoy*);                   // 0x49 LD C,C
extern void (*LoadRegEFromRegC)(GameBoy*);                   // 0x59 LD E,C
extern void (*LoadRegLFromRegC)(GameBoy*);                   // 0x69 LD L,C
extern void (*LoadRegAFromRegC)(GameBoy*);                   // 0x79 LD A,C
extern void (*AddWithCarryRegAFromRegC)(GameBoy*);           // 0x89 ADC A,C
extern void (*SubWithCarryRegAFromRegC)(GameBoy*);           // 0x99 SBC A.C
extern void (*XorRegC)(GameBoy*);                            // 0xA9 XOR C
extern void (*CompareRegC)(GameBoy*);                        // 0xB9 CP C
extern void (*Return)(GameBoy*);                             // 0xC9 RET
extern void (*ReturnIndex)(GameBoy*);                        // 0xD9 RETI
extern void (*JumpToIndirectRegHL)(GameBoy*);                // 0xE9 JP (HL)
extern void (*LoadStackPointerFromRegHL)(GameBoy*);          // 0xF9 LD SP,HL

// Nibble A
extern void (*LoadRegAFromIndirectRegBC)(GameBoy*);          // 0x0A LD A,(BC) 
extern void (*LoadRegAFromIndirectRegDE)(GameBoy*);          // 0x1A LD A,(DE)
extern void (*LoadRegAFromIncrementIndirectRegHL)(GameBoy*); // 0x2A LD A,(HL+)
extern void (*LoadRegAFromDecrementIndirectRegHL)(GameBoy*); // 0x3A LD A,(HL-)
extern void (*LoadRegCFromRegD)(GameBoy*);                   // 0x4A LD C,D
extern void (*LoadRegEFromRegD)(GameBoy*);                   // 0x5A LD E,D
extern void (*LoadRegLFromRegD)(GameBoy*);                   // 0x6A LD L,D
extern void (*LoadRegAFromRegD)(GameBoy*);                   // 0x7A LD A,D
extern void (*AddWithCarryRegAFromRegD)(GameBoy*);           // 0x8A ADC A,(HL)
extern void (*SubWithCarryRegAFromRegD)(GameBoy*);           // 0x9A SBC A,(HL)
extern void (*XorRegD)(GameBoy*);                            // 0xAA XOR (HL)
extern void (*CompareRegD)(GameBoy*);                        // 0xBA CP (HL)
extern void (*JumpIfZeroAddress16)(GameBoy*);                // 0xCA ADC A,d8
extern void (*JumpIfCarryAddress16)(GameBoy*);               // 0xDA SBC A,d8
extern void (*LoadAddress16FromRegA)(GameBoy*);              // 0xEA XOR d8
extern void (*LoadRegAFromAddress16)(GameBoy*);              // 0xFA CP d8

// Nibble B
extern void (*DecrementRegBC)(GameBoy*);                     // 0x0B DEC BC
extern void (*DecrementRegDE)(GameBoy*);                     // 0x1B DEC DE
extern void (*DecrementRegHL)(GameBoy*);                     // 0x2B DEC HL
extern void (*DecrementStackPointer)(GameBoy*);              // 0x3B DEC SP
extern void (*LoadRegCFromE)(GameBoy*);                      // 0x4B LD C,E
extern void (*LoadRegEFromE)(GameBoy*);                      // 0x5B LD E,E
extern void (*LoadRegLFromE)(GameBoy*);                      // 0x6B LD L,E
extern void (*LoadRegAFromE)(GameBoy*);                      // 0x7B LD A,E
extern void (*AddWithCarryRegAFromRegE)(GameBoy*);           // 0x8B ADC A,E
extern void (*SubWithCarryRegAFromRegE)(GameBoy*);           // 0x9B SBC A,E
extern void (*XorRegE)(GameBoy*);                            // 0xAB XOR E
extern void (*CompareRegE)(GameBoy*);                        // 0xBB CP E
extern void (*Prefix)(GameBoy*);                             // 0xCB PREFIX CB
extern void (*NoInstructionDB)(GameBoy*);                    // 0xDB NUL
extern void (*NoInstructionEB)(GameBoy*);                    // 0xEB NUL
extern void (*EnableInterrupts)(GameBoy*);                   // 0xFB EI

// Nibble C
extern void (*IncrementRegC)(GameBoy*);                      // 0x0C LD C,d8
extern void (*IncrementRegE)(GameBoy*);                      // 0x1C LD E,d8
extern void (*IncrementRegL)(GameBoy*);                      // 0x2C LD L,d8
extern void (*IncrementRegA)(GameBoy*);                      // 0x3C LD A,d8
extern void (*LoadRegCFromRegH)(GameBoy*);                   // 0x4C LD C,(HL)
extern void (*LoadRegEFromRegH)(GameBoy*);                   // 0x5C LD E,(HL)
extern void (*LoadRegLFromRegH)(GameBoy*);                   // 0x6C LD L,(HL)
extern void (*LoadRegAFromRegH)(GameBoy*);                   // 0x7C LD A,(HL)
extern void (*AddWithCarryRegAFromRegH)(GameBoy*);           // 0x8C ADC A,(HL)
extern void (*SubWithCarryRegAFromRegH)(GameBoy*);           // 0x9C SBC A,(HL)
extern void (*XorRegH)(GameBoy*);                            // 0xAC XOR (HL)
extern void (*CompareH)(GameBoy*);                           // 0xBC CP (HL)
extern void (*CallIfZeroAddress16)(GameBoy*);                // 0xCC ADC A,d8
extern void (*CallIfCarryAddress16)(GameBoy*);               // 0xDC SBC A,d8
extern void (*NoInstructionEC)(GameBoy*);                    // 0xEC XOR d8
extern void (*NoInstructionFC)(GameBoy*);                    // 0xFC CP d8

// Nibble D
extern void (*DecrementRegC)(GameBoy*);                      // 0x0D DEC C
extern void (*DecrementRegE)(GameBoy*);                      // 0x1D DEC E
extern void (*DecrementRegL)(GameBoy*);                      // 0x2D DEC L
extern void (*DecrementRegA)(GameBoy*);                      // 0x3D DEC A
extern void (*LoadRegCFromRegL)(GameBoy*);                   // 0x4D LD C,L
extern void (*LoadRegEFromRegL)(GameBoy*);                   // 0x5D LD E,L
extern void (*LoadRegLFromRegL)(GameBoy*);                   // 0x6D LD L,L
extern void (*LoadRegAFromRegL)(GameBoy*);                   // 0x7D LD A,L
extern void (*AddWithCarryRegAFromRegL)(GameBoy*);           // 0x8D ADC A,L
extern void (*SubWithCarryRegAFromRegL)(GameBoy*);           // 0x9D SBC A,L
extern void (*XorRegL)(GameBoy*);                            // 0xAD XOR L
extern void (*CompareRegL)(GameBoy*);                        // 0xBD CP L
extern void (*CallAddress16)(GameBoy*);                      // 0xCD CALL a16
extern void (*NoInstructionDD)(GameBoy*);                    // 0xDD NUL DD
extern void (*NoInstructionED)(GameBoy*);                    // 0xED NUL ED
extern void (*NoInstructionFD)(GameBoy*);                    // 0xFD NUL FD

// Nibble E
extern void (*LoadRegCImmediate8)(GameBoy*);                 // 0x0E LD C,d8
extern void (*LoadRegEImmediate8)(GameBoy*);                 // 0x1E LD E,d8
extern void (*LoadRegLImmediate8)(GameBoy*);                 // 0x2E LD L,d8
extern void (*LoadRegAImmediate8)(GameBoy*);                 // 0x3E LD A,d8
extern void (*LoadRegCIndirectHL)(GameBoy*);                 // 0x4E LD C,(HL)
extern void (*LoadRegEIndirectHL)(GameBoy*);                 // 0x5E LD E,(HL)
extern void (*LoadRegLIndirectHL)(GameBoy*);                 // 0x6E LD L,(HL)
extern void (*LoadRegAIndirectHL)(GameBoy*);                 // 0x7E LD A,(HL)
extern void (*AddWithCarryRegAFromIndirectRegHL)(GameBoy*);  // 0x8E ADC A,(HL)
extern void (*SubWithCarryRegAFromIndirectRegHL)(GameBoy*);  // 0x9E SBC A,(HL)
extern void (*XorRegL)(GameBoy*);                            // 0xAE XOR (HL)
extern void (*CompareRegL)(GameBoy*);                        // 0xBE CP (HL)
extern void (*AddWithCarryRegAFromImmediate8)(GameBoy*);     // 0xCE ADC A,d8
extern void (*SubWithCarryRegAFromImmediate8)(GameBoy*);     // 0xDE SBC A,d8
extern void (*XorRegImmediate8)(GameBoy*);                   // 0xEE XOR d8
extern void (*CompareImmediate8)(GameBoy*);                  // 0xFE CP d8

// Nibble F
extern void (*RotateRightCarryRegA)(GameBoy*);               // 0x0F RRCA
extern void (*RotateLeftRegA)(GameBoy*);                     // 0x1F RRA
extern void (*ComplementAccumulator)(GameBoy*);              // 0x2F CPL
extern void (*ComplementCarryFlag)(GameBoy*);                // 0x3F CCF
extern void (*LoadCRegA)(GameBoy*);                          // 0x4F LD C,A
extern void (*LoadERegA)(GameBoy*);                          // 0x5F LD E,A
extern void (*LoadLRegA)(GameBoy*);                          // 0x6F LD L,A
extern void (*LoadARegA)(GameBoy*);                          // 0x7F LD A,A
extern void (*AddWithCarryRegAFromRegA)(GameBoy*);           // 0x8F ADC A,A
extern void (*SubWithCarryRegAFromRegA)(GameBoy*);           // 0x9F SBC A,A
extern void (*XorRegA)(GameBoy*);                            // 0xAF XOR A
extern void (*CompareRegA)(GameBoy*);                        // 0xBF CP A
extern void (*RestartAtAddress08)(GameBoy*);                 // 0xCF RST 08H
extern void (*RestartAtAddress18)(GameBoy*);                 // 0xDF RST 18H
extern void (*RestartAtAddress28)(GameBoy*);                 // 0xEF RST 28H
extern void (*RestartAtAddress38)(GameBoy*);                 // 0xFF RST 38H
}// namespace retro::gb::instructionset