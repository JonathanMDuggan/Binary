#pragma once
// If I don't define this, C++20 spams build fmt/spdlog warnings
// because C++20 hates how fmt/spdlog is programmed.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include "include/gbengine.h"
#include "../drivers/include/peripherals_sdl.h"
#include "../drivers/include/renderer_vulkan.h"
#include "../emulation/include/gb_emulator.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h" 
using namespace gbengine;
int main(int argc, char** argv) {
  gbengine::Application app = {};
  bool running = true;
  app.name = "GameBoy_Engine";
  app.height = 600;
  app.width = 800;
  app.version = 0x00000001;

  // When the user starts the program SDL, Vulkan, and ImGui begin its
  // initialization phase. If this phase fails the program crashes
  // and returns an error.
  gbengine::SDL sdl(app);

  // The Vulkan Object also Inits ImGui
  gbengine::Vulkan vulkan(&sdl, app);

  while (running) {
    // After SDL, Vulkan, and ImGui have finshed the initialization phase,
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

    vulkan.DrawFrame(sdl.window_, &sdl.event_); 
    ImGui_ImplSDL2_ProcessEvent(&sdl.event_);
  }
  vkDeviceWaitIdle(vulkan.logical_device_);
  return EXIT_SUCCESS;
}