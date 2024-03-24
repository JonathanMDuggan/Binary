#pragma once
// If I don't define this, C++20 spams build fmt/spdlog warnings
// because C++20 hates how fmt/spdlog is programmed.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include "include/gbengine.h"

int main(int argc, char** argv) { 
  gbengine::GameBoyEngine gb_engine;
  bool running = true;
  while (running) {
    gb_engine.SDLPoolEvents(&running);
  }
  gb_engine.~GameBoyEngine();
  return EXIT_SUCCESS;
}