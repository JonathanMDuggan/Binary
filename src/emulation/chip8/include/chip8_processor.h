#include <cstdint>
#include <array>
namespace retro::chip8 {
enum Constants {
  // Sizes of Registers and Memory
  k_MaxNumberOfGeneralPerposeRegisters = 16,
  k_MaxRAMSize = 4096,
  k_MaxStackSize = 16,
  // Chip8 screen specs is 64 by 32 pixels
  k_ScreenLength = 64,
  k_ScreenHeight = 32,
  k_FontSetSize = 80,
  k_Background = 0,
  k_Foreground = 0xFFFFFFFF,

  // General purpose registers
  k_V0 = 0, k_V1 = 1, k_V2 = 2, k_V3 = 3,  k_V4 = 4,  k_V5 = 5,  k_V6 = 6, 
  k_V7 = 7, k_V8 = 8, k_V9 = 9, k_VA = 10, k_VB = 11, k_VD = 13, k_VE = 14,
  k_VF = 15,

  // Memory Locations
  k_StackBase = 0x200,
  k_FontSetAddress = 0x00,
  k_NothingSpace = 0x80,
  // Memory Space
  k_MaxROMSize = 0xDFE,
  // Input Output
  k_Keyboard = 16,

  // program counter
  k_NextInstruction = 2,
  k_SkipNextInstruction = 4,

  k_InstructionsPerSecond = 700,
  k_ScreenRefreshRate = 60
};

enum Chip8_InputMask {
  k_KeyPad0 = 0x0001, k_KeyPad1 = 0x0002, k_KeyPad2 = 0x0004,
  k_KeyPad3 = 0x0008, k_KeyPad4 = 0x0010, k_KeyPad5 = 0x0020,
  k_KeyPad6 = 0x0040, k_KeyPad7 = 0x0080, k_KeyPad8 = 0x0100,
  k_KeyPad9 = 0x0200, k_KeyPadA = 0x0400, k_KeyPadB = 0x0800,
  k_KeyPadC = 0x1000, k_KeyPadD = 0x2000, k_KeyPadE = 0x4000,
  k_KeyPadF = 0x8000, k_KeyPadNull = 0xCC
};
typedef struct Register {
 public:
  uint16_t index_{};
  uint16_t program_counter_{};
  uint8_t stack_pointer_{};
  std::array<uint8_t, k_MaxNumberOfGeneralPerposeRegisters> general_purpose_{};

  // Points to the 16th value in the general purpose array.
  // The 16th register or VF is used as a status register sometimes
  uint8_t* status_;
  uint8_t delay_timer_{};
  uint8_t sound_timer_{};
  Register();
} Register;

typedef struct Chip8 {
 public:
  uint16_t opcode_{};
  Register* reg_{};
  std::array<uint16_t, k_MaxStackSize> stack_{};
  std::array<uint8_t, k_MaxRAMSize> memory_{};
  std::array<std::array<uint32_t, k_ScreenLength>, k_ScreenHeight> screen{};
  uint8_t draw_flag_{};
  uint16_t input_{};
  Chip8(Register* reg_);
}Chip8;
}