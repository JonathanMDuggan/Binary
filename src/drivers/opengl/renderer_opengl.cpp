#include "../include/renderer_opengl.h"
gbengine::OpenGL::OpenGL(SDL* sdl) { 
  Init(sdl);
  InitIMGUI();
}
void gbengine::OpenGL::DrawFrame() {
  glViewport(0, 0, width_, height_);
  glClearColor(0.2f, 0.2f, 0.2f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(sdl_->window_); 
}
void gbengine::OpenGL::Init(SDL* sdl) {
  sdl_ = sdl;
  context_ = SDL_GL_CreateContext(sdl_->window_);
  SDL_GetWindowSize(sdl_->window_, &width_, &height_);
}