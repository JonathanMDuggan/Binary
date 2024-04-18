
#pragma once
#include "../include/peripherals_sdl.h"
#include "../../main/include/gbengine.h"
#include <iostream>
// SDL Stuff

void gbengine::SDL::Init(Application app) {
  // We're using SDL for the window creation and inputs
  //
  // On startup the application checks the configuration files
  // and init the options listed.
  SDL_WindowFlags renderer;
  switch (app.renderer) {
    case k_OpenGL:
      renderer = SDL_WINDOW_OPENGL;
      break;
    case k_Vulkan:
      renderer = SDL_WINDOW_VULKAN;
      break;
    default:
      renderer = SDL_WINDOW_HIDDEN; // place holder

      spdlog::critical(
          "No renderer in configuration file, cannot display"
          "anything to the screen");

      throw std::runtime_error(
          "no renderer in configuration file,"
          "cannot display anything to the screen");
  }
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
  window_ = SDL_CreateWindow(app.name, SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED, app.width, app.height,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |  renderer
                            );
  // SDL Image
  int img_flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP;
  img_flags |= IMG_INIT_AVIF | IMG_INIT_JXL;
  IMG_Init(img_flags);
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
        std::cout << "Bazinga!";
        *running = false;
        return;
      default:
        break;
    }
  }
}

gbengine::SDL::SDL(Application app) { 
  Init(app);
  LoadApplicationIcon();
  return;
}

void gbengine::SDL::InitTextureFromPath(const char* path_to_texture) {
  texture_ = IMG_LoadTexture(renderer_, path_to_texture);
  if (texture_ == nullptr) {
    spdlog::critical("Failed to create sdl texture {}", SDL_GetError());
  }
  // Store the configurations into the info struct
  SDL_QueryTexture(texture_, 
                   &texture_info_.format_, 
                   &texture_info_.access_, 
                   &texture_info_.width_,
                   &texture_info_.height_);
}

void gbengine::SDL::GetTextureDimensions(uint32_t* width, uint32_t* height) {
  if (texture_ == nullptr) {
    spdlog::error(
        "Get Texture Dimension was called, yet texture isn't initialized");
    return;
  }
  *width = static_cast<uint32_t>(texture_info_.width_);
  *height = static_cast<uint32_t>(texture_info_.height_);
  return;
}

SDL_Texture* gbengine::SDL::GetTexture() {
  if (texture_ == nullptr) {
    spdlog::error("Get Texture was called, yet texture isn't initialized");
    return nullptr;
  }
  return texture_;
}
void gbengine::SDL::InitSurfaceFromPath(const char* path_to_texture,
                                        File file_type) {
  if (surface_ != nullptr) {
    spdlog::warn(
        "Surface was init when Init Surface From Path function was "
        "called. You should destory the surface before using it again");
  }
  read_write_ = SDL_RWFromFile(path_to_texture, "rb");
  if (!read_write_) {
    spdlog::error("Can't find {}", path_to_texture);
    return;
  }
  // FIXME: Go to the IMG_Init() and add the flags for it this won't support 
  // all the formats because you cannot find the flags for them.
  switch (file_type) {
    case File::PNG:  surface_ = IMG_LoadPNG_RW(read_write_);    break;
    case File::JPEG: surface_ = IMG_LoadJPG_RW(read_write_);    
      surface_ = SDL_ConvertSurfaceFormat(surface_, SDL_PIXELFORMAT_RGBA32, 0);
      break;
    case File::BMP:  surface_ = SDL_LoadBMP_RW(read_write_, 1); break;
    case File::GIF:  surface_ = IMG_LoadGIF_RW(read_write_);    break;
    case File::TIF:  surface_ = IMG_LoadTIF_RW(read_write_);    break;
    case File::XPM:  surface_ = IMG_LoadXPM_RW(read_write_);    break;
    case File::TGA:  surface_ = IMG_LoadTGA_RW(read_write_);    break;
    case File::LBM:  surface_ = IMG_LoadLBM_RW(read_write_);    break;
    case File::PCX:  surface_ = IMG_LoadPCX_RW(read_write_);    break;
    case File::PNM:  surface_ = IMG_LoadPNM_RW(read_write_);    break;
    case File::SVG:  surface_ = IMG_LoadSVG_RW(read_write_);    break;
    case File::WEBP: surface_ = IMG_LoadWEBP_RW(read_write_);   break;
  }

  if (!surface_) {
    spdlog::error("Failed to load resource to surface");
    return;
  }
  SDL_RWclose(read_write_);
}

void gbengine::SDL::GetTextureInfo(uint32_t* width, uint32_t* height,
                                   uint32_t* access, uint32_t* format) {
  if (texture_ == nullptr) { 
    spdlog::error( 
        "Get Texture Info was called, yet texture isn't initialized");
    return;
  }
  if (width != nullptr)  *width = static_cast<uint32_t>(texture_info_.width_); 
  if (height != nullptr) *height = static_cast<uint32_t>(texture_info_.height_);
  if (format != nullptr) *format = texture_info_.format_;
  if (access != nullptr) *access = static_cast<uint32_t>(texture_info_.access_);
}
void gbengine::SDL::ClearTexture() {
  if (texture_ == nullptr) {
    spdlog::error("Clear Texture was called, yet texture isn't initialized");
  }
  // Do it anyway since nothing bad would happen if the if statement is true
  texture_info_ = {};
  SDL_DestroyTexture(texture_);
}
gbengine::SDL::~SDL() {
  if (texture_ != nullptr) SDL_DestroyTexture(texture_);
  if (renderer_ != nullptr) SDL_DestroyRenderer(renderer_);
  if (surface_ != nullptr) SDL_FreeSurface(surface_);
  if (window_ != nullptr) SDL_DestroyWindow(window_);
  SDL_Quit();
}
void gbengine::SDL::LoadApplicationIcon() { 
  read_write_ = SDL_RWFromFile("resources/logo/logo.png", "rb");
  
  if (read_write_ == NULL) { 
    spdlog::error("Failed to load application logo");
  }
  surface_ = IMG_LoadPNG_RW(read_write_);
  SDL_SetWindowIcon(window_, surface_);
}