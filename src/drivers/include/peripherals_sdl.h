#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"
#include "../../main/include/gbengine.h"
#include "../../types/include/enums.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
namespace binary {
class SDL {
  typedef struct Texture {
    int height_{};
    int width_{};
    int access_{};
    uint32_t format_{};
  } Texture;
  Texture texture_info_;
  uint32_t* pCount{};
  const char** pNames{};
  SDL_RWops* read_write_;
 public:
  SDL_Event event_;
  SDL_Window* window_{};
  SDL_Surface* surface_{};
  SDL_Texture* texture_{};
  SDL_Renderer* renderer_{};
  void LoadApplicationIcon();
  void Init(Application app);
  void PoolEvents(bool* running);
  SDL(Application app);
  void InitTextureFromPath(const char* path_to_texture);
  void GetTextureDimensions(uint32_t* width,uint32_t* height);
  void GetTextureInfo(uint32_t* width, uint32_t* height, uint32_t* channels,
                      uint32_t* format); 
  void InitSurfaceFromPath(const char* path_to_texture, File file_type);
  SDL_Texture* GetTexture();
  void ClearTexture();
  ~SDL();
};
}  // namespace binary 