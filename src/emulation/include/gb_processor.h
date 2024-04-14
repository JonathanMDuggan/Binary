#pragma once
#include "gb_emulator.h"
#include <cstdint>
#include <string>
#include <vector>
namespace gbengine {
 enum MemoryConstants{
    kProgramStart = 0x0100
};
typedef class SM83 {
 public:
  typedef struct Reg {
    // 8 Bit general  purpose Registers
    uint8_t A_;
    uint8_t F_;
    uint8_t B_;
    uint8_t C_;
    uint8_t D_;
    uint8_t E_;
    uint8_t H_;
    uint8_t L_;
    //
    uint16_t program_counter_;
    uint8_t instruction_;
    uint16_t stack_pointer_;
    uint8_t accumulator_;
    uint8_t interrupt_;
    uint16_t IDU_;
  } Reg;
  SM83(gbVersion version);
  Reg reg; 
 private:
  ~SM83();
  void InitSM83();
  gbVersion version_;

};
}  // namespace gbengine
