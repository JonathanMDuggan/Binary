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

TEST(GoogleTest, AssertionTrue) { ASSERT_TRUE(true); }

int main(int argc, char** argv) {
  // Initialize Google Test
  retro::Result result;
  ::testing::InitGoogleTest(&argc, argv);
#ifdef RETRO_TEST
  return RUN_ALL_TESTS();
#else
  // If it doesn't have any arguments, run the program normally.
  bool google_test_enabled = false;
  if (argc > 1) {
    spdlog::level::debug;
    spdlog::level::trace;
    std::string argument_one = argv[1];
    // Before we begin the application we check if Retro has google test enabled
    // List out the arguments to the console for debugging

    for (size_t i = 0; i < argc; i++) {
      spdlog::info("argv[{}] = {}", i, argv[i]);
    }

    if (argument_one.compare("--gtest_list_tests") == 0 ||
        argument_one.compare("--gtest_filter")     == 0 ||
        argument_one.compare("--gtest_output")     == 0 ||
        argument_one.compare("--gtest_repeat")     == 0 ||
        argument_one.compare("--gtest_shuffle")    == 0 ||
        argument_one.compare("--gtest_break_on_failure") == 0) {
      google_test_enabled = true;
      spdlog::info("Starting Google Test");
    } else {
      spdlog::critical("Arguments you've passed doesn't exist: {}", argv[1]);
      spdlog::critical("Do you mean?: '--gtest_list_tests' or other"
        " Google Test args");
      return EXIT_FAILURE;
    }
  }

  if (google_test_enabled) {
    return RUN_ALL_TESTS();
  }

  retro::Application app{}; 
  result = retro::LoadMainConfig("config/main/retro_config.yaml", &app);

  if (result != retro::k_Success) {
    return EXIT_FAILURE;
  }

  bool running = true;
  
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
  texture.Destroy();
  return EXIT_SUCCESS;
  #endif
}
