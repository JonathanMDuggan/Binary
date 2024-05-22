
#include"chip8_processor.h"
#include "../../../drivers/include/peripherals_sdl.h"
namespace binary::chip8 {
enum Opcodes {
  k_SysAddr_0x000 = 0x000,
  k_JPaddr_0x100 = 0x100,
  k_CLS_0x00E0 = 0x00E0,
  // Opcodes which start with the number 0
  CLS = 0xE0,
  RET = 0xEE,
  // Opcodes where the first number is between 1 to 7
  k_JPaddr = 1,
  k_CALLaddr = 2,
  k_SEVxbyte = 3,
  k_SNEVxbyte = 4,
  k_SEVxVy = 5,
  k_LDVxbyte = 6,
  k_ADDVxbyte = 7,
  // Opcodes which start with the number 8
  k_LDVxVy = 0,
  k_ORVxVy = 1,
  k_ANDVxVy = 2,
  k_XORVxVy = 3,
  k_ADDVxVy = 4,
  k_SUBVxVy = 5,
  k_SHRVxVy = 6,
  k_SUBNVxVy = 7,
  k_SHLVxVy = 0xE,
  // Opcodes between 9 and D
  k_SNEVxVy = 0x9,
  k_LDIaddr = 0xA,
  k_JPV0addr = 0xB,
  k_RNDVxbyte = 0xC,
  k_DRWVxVyNibble = 0xD,
  k_E = 0xE,
  k_SKPVx = 0x9E,
  k_SKNPVx = 0xA1,
  // Opcodes which start with the number F
  k_LDVxDT = 0x07,
  k_LDVxK = 0x0A,
  k_LDDTVx = 0x15,
  k_LDSTVx = 0x18,
  k_ADDIVx = 0x1E,
  k_LDFVx = 0x29,
  k_LDBVx = 0x33,
  k_LDIVx = 0x55,
  k_LDVxI = 0x65,

  // Function Pointer Size
  k_Opcode8LargestIdentifier = 0xF,
  k_OpcodeNNNLargestIdentifier = 0xB,
  k_OpcodeStartsWith8NOP = 0xA,
  k_OpcodeNNNNOP = 3,
  k_MaxBufferSize = 50,
  k_OpcodeKKLargestIdentifier = 0xC,
  k_OpcodeKKNOP = 5,
  k_OpcodeFLargestIdentifier = 0x65,
  k_OpcodeXYLargestIdentifier = 0xD,
  k_OpcodeXLargestIdentifier = 0xA1,
};

typedef void (*OpcodeFunction)(Chip8*, uint16_t);

typedef struct OpcodeHandler {
  OpcodeFunction instruction;
  const char* kAssembly;
} OpcodeHandler;

// Instruction Set functions:
//
// Unconventional instruction naming convention, so here is the explanation:
//
// The number at the end of every instruction represents the opcodes
// hexadecimal value. Whenever you see any letters outside of the hexadecimal
// space ( letter after F) that being NNN, x, or kk it means the following...
//
// * NNN: The value NNN is the address to the location in the opcode
//
// * kk:  Store that values into Register 'X', 'X' being any of the 16 registers
//        values in the Chip-8
//
// * x:   The hexadecimal value at that location is the register number,
//        for example, if the opcode had A at that x location that means the
//        register is A.
//
// * y:   This means the opcode is calling a second register, the hexadecimal
//        located at y is the second register number.
//
// Therefore if you see the instruction 'Chip8_AddMemoryToRegisterX_7xkk' it
// means Add the value at 'kk' to Register 'x'
//
extern void ClearDisplay_00E0(Chip8* chip8, uint16_t memory);
extern void Return_00EE(Chip8* chip8, uint16_t memory);
extern void JumpToLocation_1nnn(Chip8* chip8, uint16_t memory);
extern void Call_2nnn(Chip8* chip8, uint16_t memory);

extern void SkipNextInstrucionIfRegisterXEqualMemory_3xkk(
  Chip8* chip8, uint16_t memory);

extern void SkipNextInstrucionIfRegisterXDoesNotEqualMemory_4xkk(
    Chip8* chip8, uint16_t memory);

extern void SkipNextInstrucionIfRegisterXEqualRegisterY_5xy0(
  Chip8* chip8, uint16_t memory);

extern void LoadMemoryToRegisterX_6xkk(Chip8* chip8, uint16_t memory);
extern void AddMemoryToRegisterX_7xkk(Chip8* chip8, uint16_t memory);
extern void LoadRegisterYToRegsiterX_8xy0(Chip8* chip8, uint16_t memory);
extern void LoadRegisterYToRegsiterX_8xy0(Chip8* chip8, uint16_t memory);

extern void BitwiseOrRegisterXByRegisterY_8xy1(Chip8* chip8,
                                               uint16_t memory);

extern void BitwiseAndRegisterXByRegisterY_8xy2(Chip8* chip8,
                                                uint16_t memory);

extern void BitwiseXorRegisterXByRegisterY_8xy3(Chip8* chip8,
                                                uint16_t memory);

extern void AddRegisterXByRegisterY_8xy4(Chip8* chip8, uint16_t memory);
extern void SubRegisterXByRegisterY_8xy5(Chip8* chip8, uint16_t memory);
extern void ShiftRegisterXRight_8xy6(Chip8* chip8, uint16_t memory);
extern void SubtractRegisterYbyRegisterX_8xy7(Chip8* chip8,
                                              uint16_t memory);
extern void ShiftRegisterXLeft_8xyE(Chip8* chip8, uint16_t memory);
extern void SkipIfRegisterXDoesNotEqualRegisterY_9xy0(Chip8* chip8,
                                                      uint16_t memory);
extern void StoreMemoryInIndexRegister_Annn(Chip8* chip8,
                                            uint16_t opcode);
extern void JumpToLocationInMemoryPlusRegister0_Bnnn(Chip8* chip8,
                                                     uint16_t memory);
extern void SetRegisterXToRandomByteANDMemory_Cxkk(Chip8* chip8,
                                                   uint16_t memory);
extern void Display_Dxyn(Chip8* chip8, uint16_t memory);
extern void SkipIfKeyIsPressed_Ex9E(Chip8* chip8, uint16_t memory);
extern void SkipIfKeyIsNotPressed_ExA1(Chip8* chip8, uint16_t memory);

extern void RegisterEqualDelayTimer_Fx07(Chip8* chip8, uint16_t memory);
extern void StoreKeyPressInRegisterX_Fx0A(Chip8* chip8, uint16_t memory);
extern void DelayTimerEqualRegisterX_Fx15(Chip8* chip8, uint16_t memory);
extern void SetSoundTimerToRegisterX_Fx18(Chip8* chip8, uint16_t memory);
extern void IndexPlusRegisterX_Fx1E(Chip8* chip8, uint16_t memory);
extern void IndexEqualsRegisterX_Fx29(Chip8* chip8, uint16_t memory);
extern void BCDConversion_Fx33(Chip8* chip8, uint16_t memory);
extern void IndexStoreIterator_Fx55(Chip8* chip8, uint16_t memory);
// Fills all general-purpose registers with memory addresses starting at the
// memory address stored in the index register then sets the index register
// to X
extern void IndexRegisterFill_Fx65(Chip8* chip8, uint16_t opcode);
extern void Display_Dxyn(Chip8* chip8, uint16_t memory);
extern void NOP(Chip8* chip8, uint16_t memory);
extern void Timers(Chip8* chip8, SDL* sdl);
}