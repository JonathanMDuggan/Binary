#pragma once
#include <fmt/core.h>
#include "include/gbengine.h"
#include <iostream>
#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <glm/glm.hpp>
int main(int argc, char** argv) { 
  gbengine::GameBoyEngine gb_engine;
  bool running = true;
  while (running) {
    gb_engine.SDLPoolEvents(&running);
  }
  gb_engine.~GameBoyEngine();
  return EXIT_SUCCESS;
}