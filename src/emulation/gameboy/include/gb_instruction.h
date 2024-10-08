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

#include <format>
#include <array>
#include <string>
#include <functional>
#include <bitset>
#include <type_traits>

namespace binary::gb {
enum CpuFlags {
  k_0 = 0,
  k_Z = 1,
  k_N = 1,
  k_H = 1,
  k_C = 1,
  k_Same = 2  // Nothing
};


#define BINARY_GB_EXECUTE_BIT_PREFIX(upper, lower, num)                           \
  opcode_table[BIT_##num##_##upper##].execute_ =                                \
      Bit<uint8_t,&Register::lower##_, ##num##>;                              \
  opcode_table[RES_##num##_##upper##].execute_ =                                \
      Reset<uint8_t, &Register::lower##_, ##num##>;                           \
  opcode_table[SET_##num##_##upper##].execute_ =                                \
      Set<uint8_t, &Register::lower##_, ##num##>;   

#define BINARY_GB_EXECUTE_REGISTER_INDIRECT_BYTE_PREFIX\
  opcode_table[RL__HL].execute_ =                                       \
      RotateLeft<uint16_t, &Register::hl_>;                             \
  opcode_table[RLC__HL].execute_ =                                      \
      RotateLeftCircular<uint16_t, &Register::hl_>;                     \
  opcode_table[RR__HL].execute_ =                                       \
      RotateRight<uint16_t, &Register::hl_>;                            \
  opcode_table[RRC__HL].execute_ =                                      \
      RotateRightCircular<uint16_t, &Register::hl_>;                    \
  opcode_table[SWAP__HL].execute_ =                  \
      Swap<uint16_t, &Register::hl_>; \
  opcode_table[SLA__HL].execute_ =                                      \
      ShiftLeft<uint16_t, &Register::hl_>;                              \
  opcode_table[SRA__HL].execute_ =   \
      ShiftRight<uint16_t, &Register::hl_>;

#define BINARY_GB_EXECUTE_REGISTER_INDIRECT_BIT_PREFIX(num)               \
  opcode_table[BIT_##num##__HL].execute_ =                                \
      Bit<uint16_t, &Register::hl_, ##num##>;                             \
  opcode_table[RES_##num##__HL].execute_ =                                \
      Reset<uint16_t, &Register::hl_, ##num##>;                           \
  opcode_table[SET_##num##__HL].execute_ =                                \
      Set<uint16_t, &Register::hl_, ##num##>;

#define BINARY_GB_EXECUTE_BYTE_PREFIX(upper, lower)             \
  opcode_table[RL_##upper##].execute_ =                           \
      RotateLeft<uint8_t, &Register::lower##_>;                 \
  opcode_table[RLC_##upper##].execute_ =                          \
      RotateLeftCircular<uint8_t, &Register::lower##_>;         \
  opcode_table[RR_##upper##].execute_ =                           \
      RotateRight<uint8_t, &Register::lower##_>;                \
  opcode_table[RRC_##upper##].execute_ =                          \
      RotateRightCircular<uint8_t, &Register::lower##_>;        \
  opcode_table[SWAP_##upper##].execute_ =                         \
      Swap<uint8_t, &Register::lower##_>;                       \
  opcode_table[SLA_##upper##].execute_ =                          \
      ShiftLeft<uint8_t, &Register::lower##_>;                  \
   opcode_table[SRA_##upper##].execute_ =                         \
      ShiftRight<uint8_t, &Register::lower##_>;

#define BINARY_GB_REPEAT_FOR_ALL_REGISTER_INDIRECT_BIT_PREFIX(MACRO)\
MACRO(0) MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7)

#define BINARY_GB_EXECUTE_EQUALS_LOAD_REGX_FROM_INDIRECT_REG                    \
  opcode_table[LD_A__HL].execute_ =                           \
      Load<uint8_t, &Register::a_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_B__HL].execute_ =                           \
      Load<uint8_t, &Register::b_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_C__HL].execute_ =                           \
      Load<uint8_t, &Register::c_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_D__HL].execute_ =                           \
      Load<uint8_t, &Register::d_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_E__HL].execute_ =                           \
      Load<uint8_t, &Register::e_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_H__HL].execute_ =                           \
      Load<uint8_t, &Register::h_, &Register::a_, k_RegisterIndirect>; \
  opcode_table[LD_L__HL].execute_ =                           \
      Load<uint8_t, &Register::l_, &Register::a_, k_RegisterIndirect>;

#define BINARY_GB_REPEAT_FOR_ALL_BIT_PREFIX(MACRO)\
MACRO(B,b,0) MACRO(C,c,0) MACRO(D,d,0) MACRO(E,e,0) MACRO(H,h,0) MACRO(L,l,0)\
MACRO(B,b,1) MACRO(C,c,1) MACRO(D,d,1) MACRO(E,e,1) MACRO(H,h,1) MACRO(L,l,1)\
MACRO(B,b,2) MACRO(C,c,2) MACRO(D,d,2) MACRO(E,e,2) MACRO(H,h,2) MACRO(L,l,2)\
MACRO(B,b,3) MACRO(C,c,3) MACRO(D,d,3) MACRO(E,e,3) MACRO(H,h,3) MACRO(L,l,3)\
MACRO(B,b,4) MACRO(C,c,4) MACRO(D,d,4) MACRO(E,e,4) MACRO(H,h,4) MACRO(L,l,4)\
MACRO(B,b,5) MACRO(C,c,5) MACRO(D,d,5) MACRO(E,e,5) MACRO(H,h,5) MACRO(L,l,5)\
MACRO(B,b,6) MACRO(C,c,6) MACRO(D,d,6) MACRO(E,e,6) MACRO(H,h,6) MACRO(L,l,6)\
MACRO(B,b,7) MACRO(C,c,7) MACRO(D,d,7) MACRO(E,e,7) MACRO(H,h,7) MACRO(L,l,7)\
                                                                             \
MACRO(A,a,0)\
MACRO(A,a,1)\
MACRO(A,a,2)\
MACRO(A,a,3)\
MACRO(A,a,4)\
MACRO(A,a,5)\
MACRO(A,a,6)\
MACRO(A,a,7)\

#define BINARY_GB_ALL_REG(MACRO)\
MACRO(B, b) MACRO(C, c) MACRO(D, d) MACRO(E, e) MACRO(H, h) MACRO(L, l) MACRO(A, a)
#define BINARY_GB_EXECUTE_EQUALS_LOAD_REGX_FROM_REG(upper, lower) \
  opcode_table[LD_A_##upper].execute_ = Load<uint8_t, &Register::a_,&Register::lower##_>;\
  opcode_table[LD_B_##upper].execute_ = Load<uint8_t, &Register::b_,&Register::lower##_>;\
  opcode_table[LD_C_##upper].execute_ = Load<uint8_t, &Register::c_,&Register::lower##_>;\
  opcode_table[LD_D_##upper].execute_ = Load<uint8_t, &Register::d_,&Register::lower##_>;\
  opcode_table[LD_E_##upper].execute_ = Load<uint8_t, &Register::e_,&Register::lower##_>;\
  opcode_table[LD_H_##upper].execute_ = Load<uint8_t, &Register::h_,&Register::lower##_>;\
  opcode_table[LD_L_##upper].execute_ = Load<uint8_t, &Register::l_,&Register::lower##_>;

#define BINARY_GB_EXECUTE_EQUALS_OPERATION_REG(upper, lower) \
  opcode_table[ADD_##upper].execute_ = Add<&Register::lower##_>;              \
  opcode_table[ADC_##upper].execute_ = AddWithCarry<&Register::lower##_>;     \
  opcode_table[SUB_##upper].execute_ = Sub<&Register::lower##_>;              \
  opcode_table[SBC_##upper].execute_ = SubWithCarry<&Register::lower##_>;     \
  opcode_table[AND_##upper].execute_ = And<&Register::lower##_>;              \
  opcode_table[OR_##upper].execute_ = Or<&Register::lower##_>;                \
  opcode_table[XOR_##upper].execute_ = Xor<&Register::lower##_>;              \
  opcode_table[CP_##upper].execute_ = Compare<&Register::lower##_>; 

#define BINARY_GB_EXECUTE_EQUALS_OPERATION_REG_INDIRECT                           \
  opcode_table[ADD__HL].execute_ = Add<&Register::a_, k_RegisterIndirect>;        \
  opcode_table[ADC__HL].execute_ = AddWithCarry<&Register::a_,k_RegisterIndirect>;\
  opcode_table[SUB__HL].execute_ = Sub<&Register::a_, k_RegisterIndirect>;        \
  opcode_table[SBC__HL].execute_ = SubWithCarry<&Register::a_,k_RegisterIndirect>;\
  opcode_table[AND__HL].execute_ = And<&Register::a_,k_RegisterIndirect>;         \
  opcode_table[OR__HL].execute_ = Or<&Register::a_, k_RegisterIndirect>;          \
  opcode_table[XOR__HL].execute_ = Xor<&Register::a_, k_RegisterIndirect>;        \
  opcode_table[CP__HL].execute_ = Compare<&Register::a_, k_RegisterIndirect>;
#define BINARY_GB_REPEAT_FOR_ALL_16BIT_REG(MACRO)\
MACRO(HL, hl) MACRO(BC, bc)  MACRO(DE, de) MACRO(AF, af)    

#define BINARY_GB_EXECUTE_POP_AND_PUSH(upper, lower)                                    \
  opcode_table[POP_##upper].execute_ = Pop<&Register::lower##_>;                        \
  opcode_table[PUSH_##upper].execute_ = Push<&Register::lower##_>;                      \

#define BINARY_GB_EXECUTE_DEC_AND_INC(upper, lower) \
  opcode_table[INC_##upper].execute_ = Increment<uint8_t, &Register::lower##_>;    \
  opcode_table[DEC_##upper].execute_ = Decrement<uint8_t, &Register::lower##_>;

#define BINARY_GB_EXECUTE_16BIT_DEC_AND_INC(upper, lower)\
  opcode_table[INC_##upper].execute_ = Increment<uint16_t, &Register::lower##_>; \
  opcode_table[DEC_##upper].execute_ = Decrement<uint16_t, &Register::lower##_>; 

#define BINARY_GB_EXECUTE_16BIT_DEC_AND_INC_ALL_REG(MACRO) \
MACRO(BC, bc) MACRO(DE, de) MACRO(AF, af) MACRO(SP, stack_pointer)
constexpr uint8_t k_FlagZ = 0x80;
constexpr uint8_t k_FlagN = 0x40;
constexpr uint8_t k_FlagH = 0x20;
constexpr uint8_t k_FlagC = 0x10;

constexpr uint8_t k_BitIndexZ  = 7;
constexpr uint8_t k_BitIndexN  = 6;
constexpr uint8_t k_BitIndexH  = 5;
constexpr uint8_t k_BitIndexC  = 4;

typedef struct Register {
  // 8 Bit general purpose Registers
  uint8_t a_{};
  uint8_t b_{};
  uint8_t c_{};
  uint8_t d_{};
  uint8_t e_{};
  uint8_t h_{};
  uint8_t l_{};
  std::bitset<8> f_{};

  uint16_t hl_{};
  uint16_t bc_{};
  uint16_t de_{};
  uint16_t af_{};  // Only for Pushing and Popping
  uint16_t program_counter_{};
  uint16_t stack_pointer_{};
  uint16_t IDU_{};
  uint8_t instruction_{};
  uint8_t interrupt_{};
} Register;

class GameBoy {
public:
  uint64_t cycles_{};
  uint8_t idu_{};
  uint8_t read_signal_{};
  uint8_t address_bus_{};
  uint8_t data_bus_{};
  bool branched{};
  bool cb_prefixed{}; 
  Register reg_{};
  std::array<uint8_t, 8000> memory_ = {}; 
  void ClearRegisters();
  void UpdateRegHL();
  void UpdateRegBC();
  void UpdateRegDE();
  void UpdateRegAF();
  void UpdateAll16BitReg();
  uint8_t Operand8Bit();
  uint16_t Operand16Bit();
  uint16_t ReturnAddress();
  void GetProgramCounterBytes(uint8_t& high_byte, uint8_t& low_byte);
  template <uint8_t Register::*x_>
  void UpdateRegisters();
  template <uint16_t Register::*x_>
  void UpdateRegisters(); 
 private:
  inline void Update16BitRegister(uint16_t& _16bit_reg, const uint8_t& high_reg,
                                  const uint8_t& low_reg);
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
enum Instruction : uint16_t {
//Nibble 0            Nibble 1          Nibble 2          Nibble 3
  NOP         = 0x00, LD_BC_D16 = 0x01, LD__BC_A  = 0x02, INC_BC    = 0x03,
  STOP        = 0x10, LD_DE_D16 = 0x11, LD__DE_A  = 0x12, INC_DE    = 0x13,
  JR_NZ_R8    = 0x20, LD_HL_D16 = 0x21, LD__HLp_A = 0x22, INC_AF    = 0x23,
  JR_NC_R8    = 0x30, LD_SP_D16 = 0x31, LD__HLm_A = 0x32, INC_SP    = 0x33,
  LD_B_B      = 0x40, LD_B_C    = 0x41, LD_B_D    = 0x42, LD_B_E    = 0x43,
  LD_D_B      = 0x50, LD_D_C    = 0x51, LD_D_D    = 0x52, LD_D_E    = 0x53,
  LD_H_B      = 0x60, LD_H_C    = 0x61, LD_H_D    = 0x62, LD_H_E    = 0x63,
  LD__HL_B    = 0x70, LD__HL_C  = 0x71, LD__HL_D  = 0x72, LD__HL_E  = 0x73,
  ADD_B       = 0x80, ADD_C     = 0x81, ADD_D     = 0x82, ADD_E     = 0x83,
  SUB_B       = 0x90, SUB_C     = 0x91, SUB_D     = 0x92, SUB_E     = 0x93,
  AND_B       = 0xA0, AND_C     = 0xA1, AND_D     = 0xA2, AND_E     = 0xA3,
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
  ADD_H       = 0x84, ADD_L     = 0x85, ADD__HL   = 0x86, ADD_A     = 0x87,
  SUB_H       = 0x94, SUB_L     = 0x95, SUB__HL   = 0x96, SUB_A     = 0x97,
  AND_H       = 0xA4, AND_L     = 0xA5, AND__HL   = 0xA6, AND_A     = 0xA7,
  OR_H        = 0xB4, OR_L      = 0xB5, OR__HL    = 0xB6, OR_A      = 0xB7,
  CALL_NZ_A16 = 0xC4, PUSH_BC   = 0xC5, ADD_A_D8  = 0xC6, RST_00H   = 0xC7,
  CALL_NC_A16 = 0xD4, PUSH_DE   = 0xD5, SUB_D8    = 0xD6, RST_10H   = 0xD7,
  NUL_E4      = 0xE4, PUSH_HL   = 0xE5, AND_D8    = 0xE6, RST_20H   = 0xE7,
  NUL_F4      = 0xF4, PUSH_AF   = 0xF5, OR_D8     = 0xF6, RST_30H   = 0xF7,
//Nibble 8            Nibble 9          Nibble A          Nibble B
  LD__A16_SP  = 0x08, ADD_HL_BC = 0x09, LD__A_BC  = 0x0A, DEC_BC    = 0x0B,
  JR_R8       = 0x18, ADD_HL_DE = 0x19, LD__A_DE  = 0x1A, DEC_DE    = 0x1B,
  JR_Z_R8     = 0x28, ADD_HL_HL = 0x29, LD__A_HLp = 0x2A, DEC_AF    = 0x2B,
  JR_C_R8     = 0x38, ADD_HL_SP = 0x39, LD__A_HLm = 0x3A, DEC_SP    = 0x3B,
  LD_C_B      = 0x48, LD_C_C    = 0x49, LD_C_D    = 0x4A, LD_C_E    = 0x4B,
  LD_E_B      = 0x58, LD_E_C    = 0x59, LD_E_D    = 0x5A, LD_E_E    = 0x5B,
  LD_L_B      = 0x68, LD_L_C    = 0x69, LD_L_D    = 0x6A, LD_L_E    = 0x6B,
  LD_A_B      = 0x78, LD_A_C    = 0x79, LD_A_D    = 0x7A, LD_A_E    = 0x7B,
  ADC_B       = 0x88, ADC_C     = 0x89, ADC_D     = 0x8A, ADC_E     = 0x8B,
  SBC_B       = 0x98, SBC_C     = 0x99, SBC_D     = 0x9A, SBC_E     = 0x9B,
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
  ADC_H       = 0x8C, ADC_L     = 0x8D, ADC__HL   = 0x8E, ADC_A     = 0x8F,
  SBC_H       = 0x9C, SBC_L     = 0x9D, SBC__HL   = 0x9E, SBC_A     = 0x9F,
  XOR_H       = 0xAC, XOR_L     = 0xAD, XOR__HL   = 0xAE, XOR_A     = 0xAF,
  CP_H        = 0xBC, CP_L      = 0xBD, CP__HL    = 0xBE, CP_A      = 0xBF,
  CALL_Z_A16  = 0xCC, CALL_A16  = 0xCD, ADC_A_D8  = 0xCE, RST_08H   = 0xCF,
  CALL_C_A16  = 0xDC, NUL_DD    = 0xDD, SBC_A_D8  = 0xDE, RST_18H   = 0xDF,
  NUL_EC      = 0xEC, NUL_ED    = 0xED, XOR_D8    = 0xEE, RST_28H   = 0xEF,
  NUL_FC      = 0xFC, NUL_FD    = 0xFD, CP_D8     = 0xFE, RST_38H   = 0xFF,
  
// PREFIX_CB Opcodes
// 
//Nibble 0            Nibble 1          Nibble 2          Nibble 3
  RLC_B   = 0x100, RLC_C   = 0x101, RLC_D     = 0x102, RLC_E   = 0x103,
  RL_B    = 0x110, RL_C    = 0x111, RL_D      = 0x112, RL_E    = 0x113,
  SLA_B   = 0x120, SLA_C   = 0x121, SLA_D     = 0x122, SLA_E   = 0x123,
  SWAP_B  = 0x130, SWAP_C  = 0x131, SWAP_D    = 0x132, SWAP_E  = 0x133,
  BIT_0_B = 0x140, BIT_0_C = 0x141, BIT_0_D   = 0x142, BIT_0_E = 0x143,
  BIT_2_B = 0x150, BIT_2_C = 0x151, BIT_2_D   = 0x152, BIT_2_E = 0x153,
  BIT_4_B = 0x160, BIT_4_C = 0x161, BIT_4_D   = 0x162, BIT_4_E = 0x163,
  BIT_6_B = 0x170, BIT_6_C = 0x171, BIT_6_D   = 0x172, BIT_6_E = 0x173,
  RES_0_B = 0x180, RES_0_C = 0x181, RES_0_D   = 0x182, RES_0_E = 0x183,
  RES_2_B = 0x190, RES_2_C = 0x191, RES_2_D   = 0x192, RES_2_E = 0x193,
  RES_4_B = 0x1A0, RES_4_C = 0x1A1, RES_4_D   = 0x1A2, RES_4_E = 0x1A3,
  RES_6_B = 0x1B0, RES_6_C = 0x1B1, RES_6_D   = 0x1B2, RES_6_E = 0x1B3,
  SET_0_B = 0x1C0, SET_0_C = 0x1C1, SET_0_D   = 0x1C2, SET_0_E = 0x1C3,
  SET_2_B = 0x1D0, SET_2_C = 0x1D1, SET_2_D   = 0x1D2, SET_2_E = 0x1D3,
  SET_4_B = 0x1E0, SET_4_C = 0x1E1, SET_4_D   = 0x1E2, SET_4_E = 0x1E3,
  SET_6_B = 0x1F0, SET_6_C = 0x1F1, SET_6_D   = 0x1F2, SET_6_E = 0x1F3,
//Nibble 4        Nibble 5        Nibble 6          Nibble 7
  RLC_H   = 0x104, RLC_L   = 0x105, RLC__HL   = 0x106, RLC_A   = 0x107,
  RL_H    = 0x114, RL_L    = 0x115, RL__HL    = 0x116, RL_A    = 0x117,
  SLA_H   = 0x124, SLA_L   = 0x125, SLA__HL   = 0x126, SLA_A   = 0x127,
  SWAP_H  = 0x134, SWAP_L  = 0x135, SWAP__HL  = 0x136, SWAP_A  = 0x137,
  BIT_0_H = 0x144, BIT_0_L = 0x145, BIT_0__HL = 0x146, BIT_0_A = 0x147,
  BIT_2_H = 0x154, BIT_2_L = 0x155, BIT_2__HL = 0x156, BIT_2_A = 0x157,
  BIT_4_H = 0x164, BIT_4_L = 0x165, BIT_4__HL = 0x166, BIT_4_A = 0x167,
  BIT_6_H = 0x174, BIT_6_L = 0x175, BIT_6__HL = 0x176, BIT_6_A = 0x177,
  RES_0_H = 0x184, RES_0_L = 0x185, RES_0__HL = 0x186, RES_0_A = 0x187,
  RES_2_H = 0x194, RES_2_L = 0x195, RES_2__HL = 0x196, RES_2_A = 0x197,
  RES_4_H = 0x1A4, RES_4_L = 0x1A5, RES_4__HL = 0x1A6, RES_4_A = 0x1A7,
  RES_6_H = 0x1B4, RES_6_L = 0x1B5, RES_6__HL = 0x1B6, RES_6_A = 0x1B7,
  SET_0_H = 0x1C4, SET_0_L = 0x1C5, SET_0__HL = 0x1C6, SET_0_A = 0x1C7,
  SET_2_H = 0x1D4, SET_2_L = 0x1D5, SET_2__HL = 0x1D6, SET_2_A = 0x1D7,
  SET_4_H = 0x1E4, SET_4_L = 0x1E5, SET_4__HL = 0x1E6, SET_4_A = 0x1E7,
  SET_6_H = 0x1F4, SET_6_L = 0x1F5, SET_6__HL = 0x1F6, SET_6_A = 0x1F7,
//Nibble 8        Nibble 9        Nibble A          Nibble B
  RRC_B   = 0x108, RRC_C   = 0x109, RRC_D     = 0x10A, RRC_E   = 0x10B,
  RR_B    = 0x118, RR_C    = 0x119, RR_D      = 0x11A, RR_E    = 0x11B,
  SRA_B   = 0x128, SRA_C   = 0x129, SRA_D     = 0x12A, SRA_E   = 0x12B,
  SRL_B   = 0x138, SRL_C   = 0x139, SRL_D     = 0x13A, SRL_E   = 0x13B,
  BIT_1_B = 0x148, BIT_1_C = 0x149, BIT_1_D   = 0x14A, BIT_1_E = 0x14B,
  BIT_3_B = 0x158, BIT_3_C = 0x159, BIT_3_D   = 0x15A, BIT_3_E = 0x15B,
  BIT_5_B = 0x168, BIT_5_C = 0x169, BIT_5_D   = 0x16A, BIT_5_E = 0x16B,
  BIT_7_B = 0x178, BIT_7_C = 0x179, BIT_7_D   = 0x17A, BIT_7_E = 0x17B,
  RES_1_B = 0x188, RES_1_C = 0x189, RES_1_D   = 0x18A, RES_1_E = 0x18B,
  RES_3_B = 0x198, RES_3_C = 0x199, RES_3_D   = 0x19A, RES_3_E = 0x19B,
  RES_5_B = 0x1A8, RES_5_C = 0x1A9, RES_5_D   = 0x1AA, RES_5_E = 0x1AB,
  RES_7_B = 0x1B8, RES_7_C = 0x1B9, RES_7_D   = 0x1BA, RES_7_E = 0x1BB,
  SET_1_B = 0x1C8, SET_1_C = 0x1C9, SET_1_D   = 0x1CA, SET_1_E = 0x1CB,
  SET_3_B = 0x1D8, SET_3_C = 0x1D9, SET_3_D   = 0x1DA, SET_3_E = 0x1DB,
  SET_5_B = 0x1E8, SET_5_C = 0x1E9, SET_5_D   = 0x1EA, SET_5_E = 0x1EB,
  SET_7_B = 0x1F8, SET_7_C = 0x1F9, SET_7_D   = 0x1FA, SET_7_E = 0x1FB,
//Nibble C        Nibble D        Nibble E          Nibble F
  RRC_H   = 0x10C, RRC_L   = 0x10D, RRC__HL   = 0x10E, RRC_A   = 0x10F,
  RR_H    = 0x11C, RR_L    = 0x11D, RR__HL    = 0x11E, RR_A    = 0x11F,
  SRA_H   = 0x12C, SRA_L   = 0x12D, SRA__HL   = 0x12E, SRA_A   = 0x12F,
  SRL_H   = 0x13C, SRL_L   = 0x13D, SRL__HL   = 0x13E, SRL_A   = 0x13F,
  BIT_1_H = 0x14C, BIT_1_L = 0x14D, BIT_1__HL = 0x14E, BIT_1_A = 0x14F,
  BIT_3_H = 0x15C, BIT_3_L = 0x15D, BIT_3__HL = 0x15E, BIT_3_A = 0x15F,
  BIT_5_H = 0x16C, BIT_5_L = 0x16D, BIT_5__HL = 0x16E, BIT_5_A = 0x16F,
  BIT_7_H = 0x17C, BIT_7_L = 0x17D, BIT_7__HL = 0x17E, BIT_7_A = 0x17F,
  RES_1_H = 0x18C, RES_1_L = 0x18D, RES_1__HL = 0x18E, RES_1_A = 0x18F,
  RES_3_H = 0x19C, RES_3_L = 0x19D, RES_3__HL = 0x19E, RES_3_A = 0x19F,
  RES_5_H = 0x1AC, RES_5_L = 0x1AD, RES_5__HL = 0x1AE, RES_5_A = 0x1AF,
  RES_7_H = 0x1BC, RES_7_L = 0x1BD, RES_7__HL = 0x1BE, RES_7_A = 0x1BF,
  SET_1_H = 0x1CC, SET_1_L = 0x1CD, SET_1__HL = 0x1CE, SET_1_A = 0x1CF,
  SET_3_H = 0x1DC, SET_3_L = 0x1DD, SET_3__HL = 0x1DE, SET_3_A = 0x1DF,
  SET_5_H = 0x1EC, SET_5_L = 0x1ED, SET_5__HL = 0x1EE, SET_5_A = 0x1EF,
  SET_7_H = 0x1FC, SET_7_L = 0x1FD, SET_7__HL = 0x1FE, SET_7_A = 0x1FF
};
// Reference: https://gbdev.io/pandocs/Memory_Map.html
enum class MemoryMap : uint16_t {
  k_RomBank00Start   = 0x0000, k_RomBank00End   = 0x3FFF,
  k_RomBank01NnStart = 0x4000, k_RomBank01NnEnd = 0x7FFF,
  k_VideoRamStart    = 0x8000, k_VideoRamEnd    = 0x9FFF,
  k_ExternalRamStart = 0xA000, k_ExternalRamEnd = 0xBFFF,
  k_WorkRamStart     = 0xC000, k_WorkRamEnd     = 0xCFFF,
  k_EchoRamStart     = 0xE000, k_EchoRamEnd     = 0xFDFF,
  k_OamStart         = 0xFE00, k_OamEnd         = 0xFE9F,
  k_NotUsableStart   = 0xFEA0, k_NotUsableEnd   = 0xFEFF,
  k_IoRegistersStart = 0xFF00, k_IoRegistersEnd = 0xFF7F,
  k_HighRamStart     = 0xFF80, k_HighRamEnd     = 0xFFFE,
  k_InterruptEnable  = 0xFFFF
};

// Reference: https://gbdev.io/pandocs/Memory_Map.html
enum class IORanges : uint16_t {
  k_JoypadInput          = 0xFF00,
  k_SerialTransferStart  = 0xFF01, k_SerialTransferEnd  = 0xFF02,
  k_TimerDividerStart    = 0xFF04, k_TimerDividerEnd    = 0xFF07,
  k_AudioStart           = 0xFF10, k_AudioEnd           = 0xFF26,
  k_WavePatternStart     = 0xFF30, k_WavePatternEnd     = 0xFF3F,
  k_VramBankSelect       = 0xFF4F,
  k_DisableBootRom       = 0xFF50, 
  k_VramDmaStart         = 0xFF51, k_VramDmaEnd         = 0xFF55,
  k_BgObjPalettesStart   = 0xFF68, k_BgObjPalettesEnd   = 0xFF6B,
  k_WramBankSelect       = 0xFF70
};
// Reference: https://gbdev.io/pandocs/Hardware_Reg_List.html
enum class HardwareRegistersName : uint16_t {
  k_P1Joyp   = 0xFF00, // JoyPad
  k_Sb       = 0xFF01, // Serial Transfer Data
  k_Sc       = 0xFF02, // Serial Transfer Control
  k_Div      = 0xFF04, // Divider Register
  k_Tima     = 0xFF05, // Timer Counter
  k_Tma      = 0xFF06, // Timer Modulo
  k_Tac      = 0xFF07, // Timer Control
  k_If       = 0xFF0F, // Interrupt Flag
  k_Nr10     = 0xFF10, // Sound Channel 1 Sweep
  k_Nr11     = 0xFF11, // Sound Channel 1 length timer & duty cycle
  k_Nr12     = 0xFF12, // Sound Channel 1 volume & envelope
  k_Nr13     = 0xFF13, // Sound Channel 1 period low
  k_Nr14     = 0xFF14, // Sound Channel 1 period high & control
  k_Nr21     = 0xFF16, // Sound Channel 2 length timer & duty cycle
  k_Nr22     = 0xFF17, // Sound Channel 2 volume & envelope
  k_Nr23     = 0xFF18, // Sound Channel 2 period low
  k_Nr24     = 0xFF19, // Sound Channel 2 period high & control
  k_Nr30     = 0xFF1A, // Sound Channel 3 DAC enable
  k_Nr31     = 0xFF1B, // Sound Channel 3 length timer
  k_Nr32     = 0xFF1C, // Sound Channel 3 output level
  k_Nr33     = 0xFF1D, // Sound Channel 3 period low
  k_Nr34     = 0xFF1E, // Sound Channel 3 period high & control
  k_Nr41     = 0xFF20, // Sound Channel 4 length timer
  k_Nr42     = 0xFF21, // Sound Channel 4 volume & envelope
  k_Nr43     = 0xFF22, // Sound Channel 4 frequency & randomness
  k_Nr44     = 0xFF23, // Sound Channel 4 control
  k_Nr50     = 0xFF24, // Master volume & VIN panning
  k_Nr51     = 0xFF25, // Sound panning
  k_Nr52     = 0xFF26, // Sound on/off
  k_WaveRam  = 0xFF30, // Wave RAM
  k_Lcdc     = 0xFF40, // LCD control
  k_Stat     = 0xFF41, // LCD status
  k_Scy      = 0xFF42, // Viewport Y position
  k_Scx      = 0xFF43, // Viewport X position
  k_Ly       = 0xFF44, // LCD Y coordinate
  k_Lyc      = 0xFF45, // LY compare
  k_Dma      = 0xFF46, // OAM DMA source address & start
  k_Bgp      = 0xFF47, // BG palette data
  k_Obp0     = 0xFF48, // OBJ palette 0 data
  k_Obp1     = 0xFF49, // OBJ palette 1 data
  k_Wy       = 0xFF4A, // Window Y position
  k_Wx       = 0xFF4B, // Window X position plus 7
  k_Key1     = 0xFF4D, // Prepare speed switch
  k_Vbk      = 0xFF4F, // VRAM bank
  k_Hdma1    = 0xFF51, // VRAM DMA source high
  k_Hdma2    = 0xFF52, // VRAM DMA source low
  k_Hdma3    = 0xFF53, // VRAM DMA destination high
  k_Hdma4    = 0xFF54, // VRAM DMA destination low
  k_Hdma5    = 0xFF55, // VRAM DMA length/mode/start
  k_Rp       = 0xFF56, // Infrared communications port
  k_BcpsBgpi = 0xFF68, // Background color palette specification / Background 
                       // palette index
  k_BcpdBgpd = 0xFF69, // Background color palette data / Background palette data
  k_OcpsObpi = 0xFF6A, // OBJ color palette specification / OBJ palette index
  k_OcpdObpd = 0xFF6B, // OBJ color palette data / OBJ palette data
  k_Opri     = 0xFF6C, // Object priority mode
  k_Svbk     = 0xFF70, // WRAM bank
  k_Pcm12    = 0xFF76, // Audio digital outputs 1 & 2
  k_Pcm34    = 0xFF77, // Audio digital outputs 3 & 4
  k_Ie       = 0xFFFF  // Interrupt enable
};

typedef struct FlagInfo {
  CpuFlags z_ = k_Same;
  CpuFlags n_ = k_Same;
  CpuFlags h_ = k_Same;
  CpuFlags c_ = k_Same;
} FlagInfo;

typedef class Opcode {
 public:
  std::function<std::string(GameBoy*,const uint8_t)> opcode_{}; 
  std::string mnemonic_{};
  std::uint8_t machine_cycles_{};
  std::uint8_t machine_cycles_branch_{};
  std::function<void(GameBoy*)> execute_;
  void SetMachineCycles(uint8_t machine_cycles, uint8_t machine_cycles_branch);
  void SetMachineCycles(uint8_t machine_cycles);
}Opcode;

// Reference: https://gbdev.io/pandocs/The_Cartridge_Header.html
enum class NewPublisherCodeIndex {
  k_None                                   = 0,
  k_NintendoResearchAndDevelopment1        = 1,
  k_Capcom                                 = 2,
  k_Ea                                     = 3,
  k_HudsonSoft                             = 4,
  k_BAi                                    = 5,
  k_Kss                                    = 6,
  k_PlanningOfficeWada                     = 7,
  k_PcmComplete                            = 8,
  k_SanX                                   = 9,
  k_Kemco                                  = 10,
  k_SetaCorporation                        = 11,
  k_Viacom                                 = 12,
  k_Nintendo                               = 13,
  k_Bandai                                 = 14,
  k_OceanSoftwareAcclaimEntertainment      = 15,
  k_Konami                                 = 16,
  k_HectorSoft                             = 17,
  k_Taito                                  = 18,
  k_HudsonSoft2                            = 19,
  k_Banpresto                              = 20,
  k_UbiSoft                                = 21,
  k_Atlus                                  = 22,
  k_MalibuInteractive                      = 23,
  k_Angel                                  = 24,
  k_BulletProofSoftware                    = 25,
  k_Irem                                   = 26,
  k_Absolute                               = 27,
  k_AcclaimEntertainment2                  = 28,
  k_Activision                             = 29,
  k_SammyUsaCorporation                    = 30,
  k_Konami2                                = 31,
  k_HiTechExpressions                      = 32,
  k_Ljn                                    = 33,
  k_Matchbox                               = 34,
  k_Mattel                                 = 35,
  k_MiltonBradleyCompany                   = 36,
  k_TitusInteractive                       = 37,
  k_VirginGamesLtd                         = 38,
  k_LucasfilmGames                         = 39,
  k_OceanSoftware2                         = 40,
  k_Ea2                                    = 41,
  k_Infogrames                             = 42,
  k_InterplayEntertainment                 = 43,
  k_Broderbund                             = 44,
  k_SculpturedSoftware                     = 45,
  k_TheSalesCurveLimited                   = 46,
  k_Thq                                    = 47,
  k_Accolade                               = 48,
  k_MisawaEntertainment                    = 49,
  k_Lozc                                   = 50,
  k_TokumaShoten                           = 51,
  k_TsukudaOriginal                        = 52,
  k_ChunsoftCo                             = 53,
  k_VideoSystem                            = 54,
  k_OceanSoftwareAcclaimEntertainment2     = 55,
  k_Varie                                  = 56,
  k_YonezawaSPal                           = 57,
  k_Kaneko                                 = 58,
  k_PackInVideo                            = 59,
  k_BottomUp                               = 60,
  k_KonamiYuGiOh                           = 61,
  k_Mto                                    = 62,
  k_Kodansha                               = 63
};
// TODO: Fill the PublisherCodeString with the codes corrsponding with the index
const std::string k_PublisherCodeString{"0"};
enum AddressingMode {
  k_None,
  k_Address8,   k_Address16,
  k_Direct,     k_RegisterDirect,   k_RegisterDirect16,
  k_Indexed,
  k_Immediate8, k_Immediate16,
  k_Indirect,   k_RegisterIndirect,
  k_Relative,   
  k_StackPointer,
  k_ProgramCounter, k_Interrupt
};


extern void InitPrefixTable(std::array<Opcode, 512>& opcode_table);
extern void InitOpcodeTable(std::array<Opcode, 512>&);  
extern void InitLoadInstructionsTable( 
    std::array<Opcode, 512>& opcode_table); 
extern void Init8BitArithmeticLogicRegisterDirectTable( 
    std::array<Opcode, 512>& opcode_table); 
extern void InitPushAndPop(std::array<Opcode, 512>& opcode_table);
extern void InitIncrementAndDecrement(std::array<Opcode, 512>& opcode_table);
extern void InitNullOpcodes(std::array<Opcode, 512>& opcode_table);
extern void InitNullOpcode(Opcode& opcode, uint32_t code);
extern void NullOpcode(GameBoy* gb);
extern void InitConditional(std::array<Opcode, 512>& opcode_table);
extern void InitRestart(std::array<Opcode, 512>& opcode_table);
extern void InitMiscellaneous(std::array<Opcode, 512> &opcode_table);
extern void InitBitwise(std::array<Opcode, 512>& opcode_table);
template <uint8_t Register::*x_>
inline void GameBoy::UpdateRegisters() {
  if constexpr (x_ == &Register::b_ || x_ == &Register::c_) {
    UpdateRegBC();
  } else if constexpr (x_ == &Register::d_ || x_ == &Register::e_) {
    UpdateRegDE();
  } else if constexpr (x_ == &Register::a_) {
    UpdateRegAF();
  } else {
    UpdateRegHL();
  }
}

template <uint16_t Register::*x_> 
inline void GameBoy::UpdateRegisters() { 
  if constexpr (x_ == &Register::bc_) { 
    reg_.b_ = (reg_.bc_ >> 8);
    reg_.c_ = (reg_.bc_ & 0x0F); 
  } else if constexpr (x_ == &Register::de_) {
    reg_.d_ = (reg_.de_ >> 8);
    reg_.e_ = (reg_.de_ & 0x0F);
  } else if constexpr (x_ == &Register::af_) {
    reg_.a_ = (reg_.de_ >> 8);
    reg_.f_.set(reg_.de_ & 0x0F);
  } else if constexpr (x_ == &Register::hl_) {
    reg_.h_ = (reg_.de_ >> 8);
    reg_.l_ = (reg_.de_ & 0x0F);
  }
  // any other 16 bit register arn't tied to a 8 bit register so we don't update
  // anything
}

}  // namespace binary::gb

namespace binary::gb::instructionset {
/// Function implementation for the GameBoy CPU instruction set
// Reference: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
/// Naming conventions
// * Reg:         Register
// * Immediate8:  d8 or immediate 8 bit data
// * Immediate16: d16 or immediate 16 bit data
// * RegSP:       Stack pointer
/// Notes on order of operations:
// The Register that is first mention is the location where the data is stored
// Examples:
// * LoadRegDFromRegB is equivalent to: D = B
// * AddRegD is equivalent to:  A += D
//
// Nibble 0

// Addressing different processor address modes
//
// Prefix CB Instructions
void SetFlagZ0HC(GameBoy* gb, const uint16_t k_Result, uint8_t reg,
                 const uint8_t k_Operand);
extern void SetFlagZ0HC(GameBoy* gb, const uint16_t k_Result, uint8_t reg,
                        const uint8_t k_Reg);
extern void SetFlagZ1HC(GameBoy* gb, const uint16_t k_Result,
                        const uint8_t k_Reg, const uint8_t k_Operand);
extern void SetFlagZ00C(GameBoy* gb, const uint16_t k_Result); 
extern void PrefixCB(GameBoy* gb);
extern void EnableInterrput(GameBoy* gb);
extern void DisableInterrput(GameBoy* gb);
extern void NoOperation(GameBoy* gb);
template <typename T = uint8_t, T Register::*x_>
void Swap(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const uint8_t k_HighNibble = gb->reg_.*x_ >> 4;
    const uint8_t k_LowNibble = gb->reg_.*x_ << 4;
    const uint16_t k_Result = k_HighNibble | k_LowNibble;

    gb->reg_.f_ = (k_Result != 0) ? 0 : k_FlagZ;
    gb->reg_.*x_ = k_Result;
    gb->UpdateRegisters<x_>(); 
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    const uint8_t k_HighNibble = gb->memory_[gb->reg_.*x_] >> 4;
    const uint8_t k_LowNibble = gb->memory_[gb->reg_.*x_] << 4;
    const uint16_t k_Result = k_HighNibble | k_LowNibble;

    gb->reg_.f_ = (k_Result != 0) ? 0 : k_FlagZ;
    gb->memory_[gb->reg_.*x_] = k_Result;
  }

  gb->UpdateRegAF();
}


template <typename T = uint8_t, T Register::*x_>
void RotateLeft(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const bool k_7thBit = ((gb->reg_.*x_ & 0x80) > 0);
    const bool k_IsCarryFlagSet = gb->reg_.f_[k_BitIndexC]; 
    const uint8_t k_Result = (gb->reg_.*x_ << 1) | gb->reg_.f_[k_BitIndexC]; 

    SetFlagZ00C(gb, k_Result); 
    gb->reg_.f_[k_BitIndexC] = k_7thBit; 
    gb->reg_.*x_ = k_Result; 
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    const bool k_7thBit = ((gb->memory_[gb->reg_.*x_] & 0x80) > 0);
    const bool k_IsCarryFlagSet = gb->reg_.f_[k_BitIndexC]; 
    const uint8_t k_Result = (gb->memory_[gb->reg_.*x_]<< 1) 
                             | gb->reg_.f_[k_BitIndexC];

    SetFlagZ00C(gb, k_Result);
    gb->reg_.f_[k_BitIndexC] = k_7thBit; 
    gb->memory_[gb->reg_.*x_] = k_Result; 
  }
  gb->UpdateRegAF(); 
}

template <typename T = uint8_t, T Register::*x_>
void RotateLeftCircular(GameBoy* gb) { 
  if constexpr (std::is_same_v<T, uint8_t>) {
    const bool k_7thBit = ((gb->reg_.*x_ & 0x80) > 0);
    const uint8_t k_Result = (gb->reg_.*x_ << 1) | static_cast<uint8_t>(k_7thBit);

    SetFlagZ00C(gb, k_Result); 
    gb->reg_.f_[k_BitIndexC] = k_7thBit; 
    gb->reg_.*x_ = k_Result;  
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) { 
    const bool k_7thBit = ((gb->memory_[gb->reg_.*x_] & 0x80) > 0);
    const uint8_t k_Result =
        (gb->memory_[gb->reg_.*x_] << 1) | static_cast<uint8_t>(k_7thBit);

    SetFlagZ00C(gb, k_Result);
    gb->reg_.f_[k_BitIndexC] = k_7thBit;
    gb->memory_[gb->reg_.*x_] = k_Result;
  }
  gb->UpdateRegAF();
}

template <typename T = uint8_t, T Register::*x_>
void RotateRight(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const bool k_FirstBit = ((gb->reg_.*x_ & 1) > 0);
    gb->reg_.*x_ >>= 1;
    gb->reg_.*x_ |= (gb->reg_.f_[k_BitIndexC] == true) ? 0x80 : 0;
    gb->reg_.f_[k_BitIndexC] = k_FirstBit;
    gb->reg_.f_[k_BitIndexH] = false;
    gb->UpdateRegisters<x_>(); 
  } else if constexpr (std::is_same_v<T, uint16_t>) { 
    const bool k_FirstBit = ((gb->memory_[gb->reg_.*x_] & 1) > 0);
    gb->memory_[gb->reg_.*x_] >>= 1; 
    gb->memory_[gb->reg_.*x_] |= (gb->reg_.f_[k_BitIndexC] == true) ? 0x80 : 0;
    gb->reg_.f_[k_BitIndexC] = k_FirstBit; 
    gb->reg_.f_[k_BitIndexH] = false; 
  }
  gb->UpdateRegAF();
}

template <typename T = uint8_t, T Register::*x_>
void RotateRightCircular(GameBoy* gb) {
  bool k_FirstBit = ((gb->reg_.*x_ & 1) > 0);
  gb->reg_.*x_ >>= 1; 
  gb->reg_.*x_ |= (k_FirstBit == true) ? 0x80 : 0; 
  gb->reg_.f_[k_BitIndexC] = k_FirstBit; 
  gb->reg_.f_[k_BitIndexH] = false;
  gb->UpdateRegisters<x_>(); 
  gb->UpdateRegAF();
}

extern void DecimalAdjust(GameBoy* gb);
extern void SetCarryFlag(GameBoy* gb);
extern void ComplementCarryFlag(GameBoy* gb);
extern void ComplementAccumulator(GameBoy* gb);
extern void RotateLeftAccumulatorCarry(
    GameBoy* gb);
extern void RotateLeftAccumulator(GameBoy* gb);
extern void RotateRightAccumulatorCarry(GameBoy* gb);
extern void RotateRightAccumulator(GameBoy* gb);

template <typename T = uint8_t, T Register::*x_>
void ShiftLeft(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    gb->reg_.*x_ <<= 1; 
    gb->reg_.f_ = (gb->reg_.*x_) ? k_FlagZ : 0;
    gb->UpdateRegisters<x_>(); 
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    gb->memory_[gb->reg_.*x_] <<= 1;  
    gb->reg_.f_ = (gb->reg_.*x_) ? k_FlagZ : 0; 
  }
  gb->UpdateRegAF(); 
}

template <typename T = uint8_t, T Register::*x_>
void ShiftRight(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    gb->reg_.*x_ >>= 1;
    gb->reg_.f_ = (gb->reg_.*x_) ? k_FlagZ : 0;
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    gb->memory_[gb->reg_.*x_] >>= 1;
    gb->reg_.f_ = (gb->reg_.*x_) ? k_FlagZ : 0;
  }
  gb->UpdateRegAF();
}

template <typename T = uint8_t, T Register::*x_, const uint8_t BitPos>
void Bit(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    const uint16_t k_Result = gb->reg_.*x_ ^ k_Bit;
    gb->reg_.f_ |= (k_Result != 0) ? k_FlagH : k_FlagZ | k_FlagH;
    gb->reg_.*x_ = k_Result;
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    const uint16_t k_Result = gb->memory_[gb->reg_.*x_] ^ k_Bit; 
    gb->reg_.f_ |= (k_Result != 0) ? k_FlagH : k_FlagZ | k_FlagH;
    gb->memory_[gb->reg_.*x_] = k_Result;

  }
  gb->UpdateRegAF();
}

template <typename T = uint8_t, T Register::*x_, const uint8_t BitPos>
void Set(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    gb->reg_.*x_ |= k_Bit;
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    gb->memory_[gb->reg_.*x_] |= k_Bit;
  }
}

template <typename T = uint8_t, T Register::*x_, const uint8_t BitPos> 
void Reset(GameBoy* gb) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    gb->reg_.*x_ &= ~k_Bit; 
    gb->UpdateRegisters<x_>();
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    const uint8_t k_Bit = (1 << BitPos);
    gb->memory_[gb->reg_.*x_] &= ~k_Bit;
  }
}
template<const uint16_t k_Address>
void Restart(GameBoy* gb) {
  // There's a problem where if the stackpointer is greater than the memeory 
  // size the program crashes, I don't know what happens when stack pointer
  // 
  gb->memory_[gb->reg_.stack_pointer_ - 1] = (gb->reg_.program_counter_ >> 8);
  gb->memory_[gb->reg_.stack_pointer_ - 2] = (gb->reg_.program_counter_);
  gb->reg_.stack_pointer_ -= 2;
  gb->reg_.program_counter_ = k_Address;
}

template <typename T = uint8_t, T Register::*x_ = &Register::a_,  
          T Register::*y_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void Load(GameBoy* gb) {

  if constexpr (std::is_same_v<T, uint16_t>) {
    if constexpr (address_mode == k_RegisterIndirect) {
      gb->reg_.a_ = gb->memory_[gb->reg_.*y_];
    } else if constexpr (address_mode == k_Indirect) {
      gb->memory_[gb->reg_.*y_] = gb->reg_.a_;
    } else if constexpr (address_mode == k_Immediate16) {
      gb->reg_.*x_ = gb->Operand16Bit();
    } else if constexpr (address_mode == k_StackPointer) {
      gb->reg_.stack_pointer_ = gb->Operand16Bit();
    } else if constexpr (address_mode == k_Address16) {
      const uint16_t k_Address = gb->Operand16Bit();
      gb->memory_[k_Address + 1] = (gb->reg_.stack_pointer_ >> 8);
      gb->memory_[k_Address] = (gb->reg_.stack_pointer_);
    }
  } else {
    if constexpr (address_mode == k_RegisterDirect) { 
      gb->reg_.*x_ = gb->reg_.*y_; 
    } else if constexpr (address_mode == k_Immediate8) { 
      gb->reg_.*x_ = gb->Operand8Bit(); 
    } else if constexpr (address_mode == k_RegisterIndirect) {
      gb->memory_[gb->reg_.hl_] = gb->reg_.*y_;
    }
  }
  gb->UpdateRegisters<x_>(); 
}

template <uint8_t Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void Add(GameBoy* gb) {
  if constexpr (address_mode == k_StackPointer) {
    const uint8_t k_StackPointer = gb->reg_.stack_pointer_;
    const uint8_t k_ImmdiateValue = gb->Operand8Bit();
    const bool k_IsHCarry =
        ((k_StackPointer & 0x0f) + (k_ImmdiateValue & 0x0f) > 0x0f);
    const uint8_t k_Result = k_StackPointer + k_ImmdiateValue;
    const bool k_IsCarry = ((k_Result & 0x100) != 0);
    gb->reg_.stack_pointer_ = k_Result;
  } else {
    const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb);
    const uint16_t k_Result = gb->reg_.a_ + k_Operand;
    SetFlagZ0HC(gb, k_Result, gb->reg_.a_, gb->reg_.*x_);
    gb->reg_.a_ = k_Result;
    gb->UpdateRegAF();
  }
}

template <uint8_t Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect> 
void AddWithCarry(GameBoy* gb) {
  uint8_t k_RegFValue = static_cast<uint8_t>(gb->reg_.f_.to_ulong());
  const uint8_t k_Result =
      k_RegFValue + gb->reg_.*x_ + gb->reg_.f_[k_BitIndexC];
  gb->reg_.f_ = k_Result;
  SetFlagZ0HC(gb, k_Result, gb->reg_.a_, gb->reg_.*x_);
  gb->UpdateRegAF();
}

template <uint8_t Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect> 
void Sub(GameBoy* gb) { 
  const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb); 
  const uint16_t k_Result = gb->reg_.a_ - k_Operand; 
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, k_Operand);
  gb->reg_.a_ = k_Result;
  gb->UpdateRegAF();
}

template <typename T = uint8_t, T Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void Increment(GameBoy* gb) {
  if constexpr (address_mode == k_RegisterDirect) {
    ++(gb->reg_.*x_);
    if constexpr (std::is_same_v<T, uint8_t>) {
      gb->UpdateRegisters<x_>(); 
    }
  }else if constexpr (address_mode == k_RegisterIndirect) {
    gb->memory_[gb->reg_.hl_] += 1; 
  }
}

template <typename T = uint8_t, T Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void Decrement(GameBoy* gb) {
  if constexpr (address_mode == k_RegisterDirect) { 
    --(gb->reg_.*x_);
    if constexpr (std::is_same_v<T, uint8_t>) {
      gb->UpdateRegisters<x_>(); 
    }
  }else if constexpr (address_mode == k_RegisterIndirect) {
    gb->memory_[gb->reg_.hl_] -= 1;
  } 
}

template <uint8_t Register::*x_, AddressingMode address_mode>
auto GetOperandValue(GameBoy* gb) {
  if constexpr (address_mode == k_RegisterDirect) {
    return gb->reg_.*x_;
  } else if constexpr (address_mode == k_RegisterIndirect) {
    return gb->memory_[gb->reg_.hl_];
  } else if constexpr (address_mode == k_Immediate8) {
    return gb->Operand8Bit();
  } else if constexpr (address_mode == k_Immediate16){
    return gb->Operand16Bit();
  } else if constexpr (address_mode == k_RegisterDirect16) {
    return gb->reg_.hl_;
  }
}

template <uint8_t Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void SubWithCarry(GameBoy* gb) {
  const uint8_t k_RegFValue = static_cast<uint8_t>(gb->reg_.f_.to_ulong());
  const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb); 
  const uint16_t k_Result = k_RegFValue - k_Operand - gb->reg_.f_[k_BitIndexC];
  gb->reg_.f_ = k_Result;
  SetFlagZ1HC(gb, k_Result, k_RegFValue, k_Operand);
  gb->UpdateRegAF();
}

template <uint8_t Register::* x_ = &Register::a_,
  AddressingMode address_mode = k_RegisterDirect>
void Xor(GameBoy* gb) {
  const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb);
  const uint16_t k_Result = gb->reg_.a_ ^ k_Operand;
  gb->reg_.f_ = (k_Result != 0) ? 0 : k_FlagZ;
  gb->reg_.a_ = k_Result;
  gb->UpdateRegAF();
}

template <uint8_t Register::*x_ = &Register::a_,
  AddressingMode address_mode = k_RegisterDirect>
void Or(GameBoy* gb) {
  const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb);
  const uint16_t k_Result = k_Operand | gb->reg_.a_;
  gb->reg_.f_ = (k_Result != 0) ? false : k_FlagZ;
  gb->reg_.a_ = k_Result;
  gb->UpdateRegAF();
}

template <uint8_t Register::*x_ = &Register::a_, 
  AddressingMode address_mode = k_RegisterDirect>
void And(GameBoy* gb) {
  const uint8_t k_Operand = GetOperandValue<x_, address_mode>(gb);
  const uint16_t k_Result = k_Operand & gb->reg_.a_;
  gb->reg_.f_ = (k_Result != 0) ? k_FlagH : (k_FlagZ | k_FlagH);
  gb->reg_.a_ = k_Result;     
  gb->UpdateRegAF();
}

template <uint8_t Register::*x_ = &Register::a_,
          AddressingMode address_mode = k_RegisterDirect>
void Compare(GameBoy* gb) {
  uint16_t k_Result = 0;
  if constexpr (address_mode == k_RegisterDirect) {
    k_Result = gb->reg_.a_ - gb->reg_.*x_; 
  } else if constexpr (address_mode == k_RegisterIndirect) {
    k_Result = gb->reg_.a_ - gb->memory_[gb->reg_.hl_];
  }
  SetFlagZ1HC(gb, k_Result, gb->reg_.a_, gb->reg_.*x_);  
  gb->reg_.f_ = k_Result; 
  gb->UpdateRegAF(); 
}

template <uint16_t Register::*x_>
void Pop(GameBoy* gb) {
  const uint8_t k_HighNibble = gb->memory_[gb->reg_.stack_pointer_ + 1];
  const uint8_t k_LowNibble = gb->memory_[gb->reg_.stack_pointer_ + 2];
  gb->reg_.*x_ = k_HighNibble | k_LowNibble;
  gb->reg_.stack_pointer_ += 2;
}

template <uint16_t Register::*x_>
void Push(GameBoy* gb) {
  const uint8_t k_HighNibble = gb->reg_.*x_ >> 4;
  const uint8_t k_LowNibble = gb->reg_.*x_ << 4;
  gb->memory_[gb->reg_.stack_pointer_] = k_LowNibble;
  gb->memory_[gb->reg_.stack_pointer_ - 1] = k_HighNibble;
  gb->reg_.stack_pointer_--;
  
}

template <uint8_t Register::*x_>
void RotateCarryRegisterDirect8(GameBoy* gb) {

}

template <const bool has_condition, const uint8_t bit_index = k_BitIndexZ,
          const bool condition = false>
void Call(GameBoy* gb) { 
  uint8_t program_coutner_high;
  uint8_t program_counter_low;
  uint16_t function_address = gb->Operand16Bit(); 
  gb->GetProgramCounterBytes(program_coutner_high, program_counter_low);
  if constexpr (has_condition) {
    if (gb->reg_.f_[bit_index] == condition) {
      gb->memory_[gb->reg_.stack_pointer_ - 1] = program_coutner_high;
      gb->memory_[gb->reg_.stack_pointer_ - 2] = program_counter_low;
      gb->reg_.program_counter_ = function_address;
      gb->branched = true;
    }
  } else {
    gb->memory_[gb->reg_.stack_pointer_ - 1] = program_coutner_high;
    gb->memory_[gb->reg_.stack_pointer_ - 2] = program_counter_low;
    gb->reg_.program_counter_ = function_address;
  }
}

template <const bool has_condition, const uint8_t bit_index = k_BitIndexZ,
          const bool condition = false>
void Return(GameBoy* gb) {

  if constexpr (has_condition) { 
    if (gb->reg_.f_[bit_index] == condition) { 
      gb->reg_.program_counter_ = gb->ReturnAddress();
      gb->branched = true;
    }
  } else {
    gb->reg_.program_counter_ = gb->ReturnAddress();
  }
}
extern void ReturnFromInterruptHandler(GameBoy* gb)
;
template <const bool has_condition, const uint8_t bit_index = k_BitIndexZ, 
          const bool condition = false>
void JumpRelative(GameBoy* gb) {
  if constexpr (has_condition) {
    if (gb->reg_.f_[bit_index] == condition) {
      gb->reg_.program_counter_ += gb->Operand8Bit(); 
      gb->branched = true; 
    }
  } else {
    gb->reg_.program_counter_ += gb->Operand8Bit(); 
  }
}
extern void JumpIndirect(GameBoy*gb);
template <const bool has_condition, const uint8_t bit_index = k_BitIndexZ,
          const bool condition = false>
void Jump(GameBoy* gb) {
  if constexpr (has_condition) {
    if (gb->reg_.f_[bit_index] == condition) {
      const uint16_t k_JumpAddress = gb->Operand16Bit();
      gb->reg_.stack_pointer_ = gb->reg_.program_counter_;
      gb->reg_.program_counter_ = k_JumpAddress;
      gb->branched = true;
    }
  } else {
    const uint16_t k_JumpAddress = gb->Operand16Bit(); 
    gb->reg_.stack_pointer_ = gb->reg_.program_counter_; 
    gb->reg_.program_counter_ = k_JumpAddress; 
  }
  // Load Instructions
}

template <const uint8_t k_OpcodeLenght = 0>
std::string PrintOpcode(GameBoy* gb, const uint8_t opcode) {
  std::string output; 
  if constexpr (k_OpcodeLenght == 2) {
    const uint8_t k_Byte = gb->memory_[opcode + 1];
    output = std::format("{:02X} {:02X}", opcode, k_Byte);
  } else if constexpr (k_OpcodeLenght == 3) {
    const uint8_t k_HByte = gb->memory_[opcode + 2];
    const uint8_t k_LByte = gb->memory_[opcode + 1];
    output = std::format("{:02X} {:02X} {:02X}", opcode, k_LByte, k_HByte);
  } else{
    output = std::format("{:02X}", opcode);
  }
  return output;
}

template <const uint8_t k_Opcode_Length,
          const uint8_t k_Machine_Cycles_Branched = 0>
extern void InitGenericOpcode(Opcode& opcode, const std::string k_Mnemonic,
                              const uint8_t k_Machine_Cycles) {
  using namespace binary::gb::instructionset;
  if constexpr (k_Machine_Cycles_Branched > 0) {
    opcode.SetMachineCycles(k_Machine_Cycles_Branched, k_Machine_Cycles);
  } else {
    opcode.SetMachineCycles(k_Machine_Cycles);
  }
  opcode.opcode_ = PrintOpcode<k_Opcode_Length>;
  opcode.mnemonic_ = k_Mnemonic;
}
// LD r, r;   
extern inline void Fetch(GameBoy* gb);

}// namespace binary::gb::instructionset