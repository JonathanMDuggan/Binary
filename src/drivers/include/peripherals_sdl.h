#pragma once
#include "../../include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
namespace gbengine {
class SDL {
  SDL_Renderer* renderer{};
  SDL_Surface* surface_{};
  uint32_t* pCount{};
  const char** pNames{};
 public:
  SDL_Event event;
  SDL_Window* window{};
 
  void Init(Application app);
  void PoolEvents(bool* running);
  SDL(Application app);
  ~SDL();
};
}  // namespace gbengine