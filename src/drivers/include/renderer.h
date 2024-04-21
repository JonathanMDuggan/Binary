#pragma once 
#include "peripherals_sdl.h"
namespace gbengine {
class Renderer {
 public:
  virtual void DrawFrame();
  virtual void StartIMGUI();
  virtual ~Renderer();
};
}
