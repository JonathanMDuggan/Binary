#pragma once
// If I don't define this, C++20 spams build fmt/spdlog warnings
// because C++20 hates how fmt/spdlog is programmed.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include "include/gbengine.h"
#include "../drivers/include/peripherals_sdl.h"
#include "../drivers/include/renderer_vulkan.h"
#include "../emulation/include/gb_emulator.h"
#include "../drivers/include/renderer_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h" 
#include "../gui/include/gb_gui.h"
using namespace gbengine;
int main(int argc, char** argv) {
  bool running = true;
  gbengine::Application app = {};
  app.name = "Reverse Studio";
  app.height = 600;
  app.width = 800;
  app.version = 0x00000001;
  app.renderer = k_Vulkan;
  // When the user starts the program SDL, Renederer, and ImGui begin 
  // its initialization phase. If this phase fails the program crashes
  // and returns an error.
  gbengine::SDL sdl(app);
  gbengine::Renderer* render;
  gbengine::GUI* gui;

  if (app.renderer == k_OpenGL) {
    render = new OpenGL(&sdl);
    gui = new OpenGLGUI;
  } else {
    render = new Vulkan(&sdl, app);
    gui = new VulkanGUI;
  }

  while (running) {
    // After SDL, Renederer, and ImGui have finshed the initialization phase,
    // The program is stuck in this main loop until the user closes the program
    SDL_Event event; 
    while (SDL_PollEvent(&event)) { 
      ImGui_ImplSDL2_ProcessEvent(&event);  
      if (event.type == SDL_QUIT) { 
        running = false;
      }
      if (event.window.event == SDL_WINDOWEVENT_CLOSE && 
          event.window.windowID == SDL_GetWindowID(sdl.window_)){
        running = false;
      }
    }
    gui->StartGUI();
    gui::mainmenu::Start();
    render->DrawFrame();
  }
  render->~Renderer();
  return EXIT_SUCCESS;
}