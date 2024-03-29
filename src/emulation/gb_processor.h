#include <cstdint>
#include <string>
#include <vector>
namespace gbengine {
  typedef enum {
    kProgramStart = 0x1000
};
typedef class SM83 {
 public:
  SM83();
  ~SM83();

 private:
   typedef struct Reg {
    // 8 Bit general  purpose Registers
    uint8_t A;
    uint8_t F;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;
    // 
    uint16_t program_counter;
    uint16_t stack_pointer;
    uint8_t accumulator;
    uint8_t instruction;
    uint8_t interrupt;
    uint16_t IDU;
  };
};

SM83::SM83() {}

SM83::~SM83() {}
}