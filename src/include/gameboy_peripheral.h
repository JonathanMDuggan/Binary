#include <array>
namespace GameBoy_Engine {
typedef enum PeripheralConstants { 
  kGBScreenHeight = 144,
  kGBScreenLength = 160
};
std::array<uint8_t> screen[kGBScreenLength][kGBScreenHeight];
  
}