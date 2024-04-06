#pragma once
#include "../main/include/gbengine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_image.h>
namespace gbengine {
class SDL {
  typedef struct Texture {
    int height_{};
    int width_{};
  } Texture;
  Texture texture_info_;
  uint32_t* pCount{};
  const char** pNames{};
 public:
  SDL_Event event_;
  SDL_Window* window_{};
  SDL_Surface* surface_{};
  SDL_Texture* texture_{};
  SDL_Renderer* renderer_{};
  void Init(Application app);
  void PoolEvents(bool* running);
  SDL(Application app);
  void GetTextureFromPath(const char* path_to_texture);
  void GetTextureDimensions(uint32_t* width,uint32_t* height);
  ~SDL();
};
}  // namespace gbengine