
#pragma once
#include "include/peripherals_sdl.h"
#include <iostream>
// SDL Stuff

void gbengine::SDL::Init(Application app) {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  window = SDL_CreateWindow(app.name, SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED, app.width,
                            app.height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}

void gbengine::SDL::PoolEvents(bool* running) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        std::cout << "Key was pressed down\n";
        break;
      case SDL_QUIT:
        *running = false;
        return;
      default:
        break;
    }
  }
}

gbengine::SDL::SDL(Application app) { 
  Init(app);
  return;
}

gbengine::SDL::~SDL() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}
