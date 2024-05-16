#include <cstdint>
namespace retro::gb {
class SM83 {
 public:
  uint64_t cycles_;
  uint8_t idu_;
  uint8_t read_signal_;
  uint8_t address_bus_;
  uint8_t data_bus_;
  typedef struct Register {
    // 8 Bit general purpose Registers
    uint8_t a_{};
    uint8_t f_{};
    uint8_t b_{};
    uint8_t c_{};
    uint8_t d_{};
    uint8_t e_{};
    uint8_t h_{};
    uint8_t l_{};

    uint16_t hl_;
    uint16_t bc_;
    uint16_t de_;
    uint16_t program_counter_{};
    uint16_t stack_pointer_{};
    uint16_t IDU_{};
    uint8_t instruction_{};
    uint8_t accumulator_{};
    uint8_t interrupt_{};
  } Register;
  typedef struct Flags {
    bool zero_;
    bool subtract_;
    bool half_carry_;
    bool carry_;
  } Flags;
  Register reg_;
  Flags flags_;
  SM83();
  void PrintCurrentProgramCounterValue();
};
extern uint32_t Test123();
}  // namespace retro::gb