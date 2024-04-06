
#pragma once
#include "include/peripherals_sdl.h"
#include "../main/include/gbengine.h"
#include <iostream>
// SDL Stuff

void gbengine::SDL::Init(Application app) {
  // We're using SDL for the window creation and inputs
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  window_ = SDL_CreateWindow(app.name, SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED, app.width,
                            app.height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  // SDL Image
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  // Create SDL renderer
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) {
    spdlog::critical("Failed to create renderer {}", SDL_GetError());
  }
}

void gbengine::SDL::PoolEvents(bool* running) {
  while (SDL_PollEvent(&event_)) {
    switch (event_.type) {
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

void gbengine::SDL::GetTextureFromPath(const char* path_to_texture) {
  this->surface_ = IMG_Load(path_to_texture);
  if (this->surface_ == nullptr) {
    spdlog::critical("Failed to create sdl surface {}", SDL_GetError());
  }
  texture_ = IMG_LoadTexture(renderer_, path_to_texture);
  if (texture_ == nullptr) {
    spdlog::critical("Failed to create sdl texture {}", SDL_GetError());
  }
  // Store the configurations into the info struct
  SDL_QueryTexture(texture_, nullptr, nullptr, &texture_info_.width_,
                   &texture_info_.height_);
}

gbengine::SDL::~SDL() {
  SDL_DestroyRenderer(renderer_);
  SDL_FreeSurface(surface_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}
