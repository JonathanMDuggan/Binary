#pragma once
#include "../include/renderer.h"
#include "imgui_impl_opengl3.h"
#include <gl/GL.h>
namespace retro {
class OpenGL : public Renderer{ 
 public:
  OpenGL(SDL* sdl);
  void DrawFrame();

 private:
  void Init(SDL* sdl);
  void InitIMGUI();
  SDL_GLContext context_{};
  int width_;
  int height_;
  SDL* sdl_;
  SDL_Window* window_;
};
}
