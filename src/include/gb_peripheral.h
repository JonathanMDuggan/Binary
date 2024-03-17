#include <array>
#include <cstdint>
namespace gbengine {
typedef enum PeripheralConstants {
  kGBScreenHeight = 144,
  kGBScreenLength = 160
};

std::array<std::array<uint8_t, kGBScreenLength>, kGBScreenHeight> screen;
}  // namespace gbengine