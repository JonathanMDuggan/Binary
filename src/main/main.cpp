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
using namespace retro;
int main(int argc, char** argv) {
  bool running = true;
  retro::Application app = {};
  app.name = "Retro";
  app.height = 720;
  app.width = 1280;
  app.version = 0x00000001;
  app.renderer = k_Vulkan;

  // When the user starts the program SDL, Renederer, and ImGui begin 
  // its initialization phase. If this phase fails the program crashes
  // and returns an error.
  retro::SDL sdl(app);
  retro::Renderer* render;
  retro::GUI* gui;
  if (app.renderer == k_OpenGL) {
    render = new retro::OpenGL(&sdl);
    gui = new OpenGLGUI;
  } else {
    render = new retro::Vulkan(&sdl, app);
    gui = new VulkanGUI;
  }

  while (running) {
    bool window_is_minimized = true;
    // After SDL, Renederer, and ImGui have finshed the initialization phase,
    // The program is stuck in this main loop until the user closes the program

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(sdl.window_)) {
        running = false;
      }
    }

    // When we minimize the window this casues vulkan to send validation
    // errors because the window size is less than 1. To fix this, we do not
    // draw new frames until the user opens the application.
    if (!(SDL_GetWindowFlags(sdl.window_) & SDL_WINDOW_MINIMIZED)) {
      gui->StartGUI(); 
      gui::mainmenu::Start(); 
      render->DrawFrame(); 
    }

  }

  render->~Renderer();
  return EXIT_SUCCESS;
}