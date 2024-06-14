#include <functional>
#include <cstdint>
#include <bitset>
namespace binary::gba {
  struct Register {
    uint32_t s; 
    uint32_t d; 
    uint32_t b; 
    uint32_t o;
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    std::bitset<32> apsr;

  };
  class Arm {
   public:
    Register reg;
  };
  struct Opcode {

};
}