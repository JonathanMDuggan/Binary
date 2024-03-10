#include <vector>
#include <array>
#include <cstdint>
class Memory {
 public:
  Memory();
  ~Memory();

 private:
  std::array<uint8_t> VRAM[0x4000];
  std::array<uint8_t> RAM[0x8000];
};

Memory::Memory() {}

Memory::~Memory() {}
