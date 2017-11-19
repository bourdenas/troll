#include "sdl/renderer.h"

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sdl/texture.h"

namespace troll {

void Renderer::Init(int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
    return;
  }

  window_ = SDL_CreateWindow("Troll the World!", 100, 100, width, height,
                             SDL_WINDOW_SHOWN);
  if (window_ == nullptr) {
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    return;
  }

  sdl_renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (sdl_renderer_ == nullptr) {
    std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    return;
  }
}

void Renderer::CleanUp() {
  SDL_DestroyRenderer(sdl_renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

std::unique_ptr<Texture> Renderer::LoadTexture(const std::string& filename) {
  constexpr char* kResourcePath = "../data/resources/";
  SDL_Texture* texture =
      IMG_LoadTexture(sdl_renderer_, (kResourcePath + filename).c_str());
  if (texture == nullptr) {
    std::cout << SDL_GetError() << std::endl;
  }
  return std::make_unique<Texture>(texture);
}

std::unique_ptr<Texture> Renderer::CreateTexture(int width, int height,
                                                 const RGBa& colour) {
  return nullptr;
}

void Renderer::BlitTexture(const Texture& src, const Box& src_box,
                           const Box& dst_box) const {
  SDL_Rect src_rect = {
      src_box.left(), src_box.top(), src_box.width(), src_box.height(),
  };
  SDL_Rect dst_rect = {
      dst_box.left(), dst_box.top(), dst_box.width(), dst_box.height(),
  };
  SDL_RenderCopy(sdl_renderer_, src.texture_,
                 src_box.width() == 0 ? nullptr : &src_rect,
                 dst_box.width() == 0 ? nullptr : &dst_rect);
}

void Renderer::Flip() const { SDL_RenderPresent(sdl_renderer_); }

void Renderer::ClearScreen() const { SDL_RenderClear(sdl_renderer_); }

}  // namespace troll
