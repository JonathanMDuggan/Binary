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
enum Instruction {
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
  NET_NC      = 0xD0, POP_DE    = 0xD1, JP_NC_A16 = 0xD2, NUL_D3    = 0xD3,
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
enum CpuFlags {
  Z = 0b1000000,
  N = 0b0100000,
  H = 0b0010000,
  C = 0b0001000
};
typedef struct Opcode {
  Instruction hexadecimal;
  std::string name;
  std::function<void()> execute(Device* gb); 

}Opcode;
extern const std::array<Opcode, 512> k_OpcodeLookupTable;
// Function implementation for 
// Nibble 0
extern void (*NOP_NoOperation)(GameBoy*);
extern void (*STOP_Stop)(GameBoy*);
extern void (*JR_NZ_R8_JumpRelativeSubtractZeroReg8)(GameBoy*);
extern void (*JR_NC_R8_JumpRelativeSubtractCarryReg8)(GameBoy*);
extern void (*LD_B_B_LoadRegBtoRegB)(GameBoy*);
extern void (*LD_B_D_LoadRegDtoRegB)(GameBoy*);
extern void (*LD_H_B_LoadRegHtoRegB)(GameBoy*);
extern void (*LD__HL_B_LoadRegHLtoRegB)(GameBoy*);
extern void (*ADD_A_B_AddRegAtoB)(GameBoy*);
extern void (*SUB_B_SubRegB)(GameBoy*);
extern void (*AND_B_AndRegB)(GameBoy*);
extern void (*RET_NZ_ReturnSubtractZero)(GameBoy*);
extern void (*RET_NC_ReturnSubtractCarry)(GameBoy*);
}  // namespace retro::gb

