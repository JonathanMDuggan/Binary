#pragma once
// If I don't define this, C++20 spams build fmt/spdlog warnings
// because C++20 hates how fmt/spdlog is programmed.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include <gtest/gtest.h>
#include <memory>
#include "include/gbengine.h"
#include "../drivers/include/peripherals_sdl.h"
#include "../drivers/include/renderer_vulkan.h"
#include "../drivers/include/renderer_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h" 
#include "../gui/include/gb_gui.h"
#include "../io/include/io.h"
int main(int argc, char** argv) {
  if (argc > 1) {
    spdlog::critical(
        "Retro doesn't take in any arguments. Closing the program...");
    return EXIT_FAILURE;
  }
  bool running = true;
  retro::Application app = retro::LoadMainConfig("config/main/retro_config.yaml");
  // When the user starts the program SDL, Renderer, and ImGui begin 
  // its initialization phase. If this phase fails the program crashes
  // and returns an error.
  retro::SDL sdl(app);
  std::unique_ptr<retro::Renderer> render;
  std::unique_ptr<retro::GUI> gui;

  if (app.renderer == retro::k_OpenGL) {
    render = std::make_unique<retro::OpenGL>(&sdl);
    gui = std::make_unique<retro::OpenGLGUI>();
  } else {
    render = std::make_unique<retro::Vulkan>(&sdl, app);
    gui = std::make_unique<retro::VulkanGUI>();
  }

  retro::gbVulkanGraphicsHandler vulkan = render->GetGraphicsHandler();
  retro::VulkanViewport texture(vulkan, &sdl);
  texture.LoadFromPath("resources/textures/sunshine.png");
  while (running) {
    bool window_is_minimized = true;
    // After SDL, Renderer, and ImGui have finished the initialization phase,
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
      if (event.key.keysym.sym == SDLK_LEFT) {
        std::cout << "AHHHHH!\n";
        sdl.InitSurfaceFromPath("resources/textures/moonvoid.png", 
          retro::File::PNG);
        texture.Update(sdl.surface_->pixels); 
      }
    }

    // When we minimize the window this causes Vulkan to send validation
    // errors because the window size is less than 1. To fix this, we do not
    // draw new frames until the user opens the application.
    if (!(SDL_GetWindowFlags(sdl.window_) & SDL_WINDOW_MINIMIZED)) {
      gui->StartGUI(); 
      retro::VulkanViewportInfo vulkan_viewport_info = texture.GetViewportInfo();
      retro::gui::mainmenu::Start(&vulkan_viewport_info); 
      render->DrawFrame(); 
    }

  }
  texture.Destory();
  return EXIT_SUCCESS;
}
