#include "../include/renderer_opengl.h"
gbengine::OpenGL::OpenGL(SDL* sdl) { 
  Init(sdl); }
void gbengine::OpenGL::DrawFrame() { 
  glViewport(0, 0, width_, height_);
  glClearColor(0.2f, 0.2f, 0.2f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window_);
}
void gbengine::OpenGL::Init(SDL* sdl) {
  window_ = sdl->window_;
  context_ = SDL_GL_CreateContext(sdl->window_);
  SDL_GetWindowSize(sdl->window_, &width_, &height_);
}