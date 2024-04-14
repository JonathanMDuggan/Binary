#pragma once
// If I don't define this, C++20 spams build fmt/spdlog warnings
// because C++20 hates how fmt/spdlog is programmed.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include "include/gbengine.h"
#include "../drivers/include/peripherals_sdl.h"
#include "../drivers/include/renderer_vulkan.h"
#include "../emulation/include/gb_emulator.h"
using namespace gbengine;
int main(int argc, char** argv) { 
  gbengine::Application app = {};
  bool running = true;
  app.name = "GameBoy_Engine";
  app.height = 600;
  app.width = 800;
  app.version = 0x00000001;
  test();
 
  gbengine::SDL sdl(app);
  gbengine::Vulkan vulkan(&sdl, app);
  while (running) {
    sdl.PoolEvents(&running);
    vulkan.DrawFrame(sdl.window_, &sdl.event_);
  }
  vkDeviceWaitIdle(vulkan.logical_device_);
  return EXIT_SUCCESS;
}