#pragma once
#include "gb_emulator.h"
#include <array>
#include <cstdint>
namespace gbengine {
typedef enum PeripheralConstants {
  kGBScreenHeight = 144,
  kGBScreenLength = 160
};

typedef struct Color {
  uint32_t lightest;
  uint32_t light;
  uint32_t dark;
  uint32_t darkest;
}Color;
typedef struct ScreenConfig {
  Color color;
}ScreenConfig;
}  // namespace gbengine