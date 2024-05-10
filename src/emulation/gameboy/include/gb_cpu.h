#include <cstdint>
#include <string>
#include <array>
#include <functional>

namespace retro::gb{
class SM83 {
public:
typedef struct Register {
  // 8 Bit general purpose Registers
  uint8_t A_{};
  uint8_t F_{};
  uint8_t B_{};
  uint8_t C_{};
  uint8_t D_{};
  uint8_t E_{};
  uint8_t H_{};
  uint8_t L_{};
  
  uint16_t program_counter_{};
  uint8_t instruction_{};
  uint16_t stack_pointer_{};
  uint8_t accumulator_{};
  uint8_t interrupt_{};
  uint16_t IDU_{};
} Register; 
typedef struct Flags {
  bool zero_;
  bool subtract_;
  bool half_carry_;
  bool carry_;
}Flags;
Register register_;
SM83();
void PrintCurrentProgramCounterValue();
};

extern uint32_t Test123();
class Device {
  SM83* sm83_;
};
}
