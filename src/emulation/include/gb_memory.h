#include <vector>
#include <array>
#include <cstdint>
class Memory {
 public:
  Memory();
  ~Memory();

 private:
  std::array<uint8_t, 0x4000> VRAM;
  std::array<uint8_t, 0x8000> RAM;
};

Memory::Memory() {}

Memory::~Memory() {}
