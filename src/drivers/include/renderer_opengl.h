#include "../include/peripherals_sdl.h"
#include "../include/renderer.h"
#include <gl/GL.h>
namespace gbengine {
class OpenGL: public Renderer{
 public:
  OpenGL(SDL* sdl);
  void DrawFrame();
 private:
  void Init(SDL* sdl);
  SDL_GLContext context_{};
  int width_;
  int height_;
  SDL* sdl_;
  SDL_Window* window_;
};
}
