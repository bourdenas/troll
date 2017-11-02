#include "sdl/renderer.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "sdl/texture.h"

namespace troll {

void Renderer::Init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
    return;
  }

  window_ = SDL_CreateWindow("Troll the World!", 100, 100, 640, 480,
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

  auto texture = LoadTexture("../data/resources/hello.bmp");
  for (int i = 0; i < 3; ++i) {
    ClearScreen();
    BlitTexture(*texture, Box(), Box());
    Flip();

    SDL_Delay(1000);
  }
}

void Renderer::CleanUp() {
  SDL_DestroyRenderer(sdl_renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

std::unique_ptr<Texture> Renderer::LoadTexture(const std::string& filename) {
  SDL_Surface* image = SDL_LoadBMP(filename.c_str());
  if (image == nullptr) {
    std::cout << SDL_GetError() << std::endl;
    return nullptr;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer_, image);
  SDL_FreeSurface(image);
  if (texture == nullptr) {
    std::cout << SDL_GetError() << std::endl;
  }
  return std::make_unique<Texture>(texture);
}

void Renderer::BlitTexture(const Texture& src, const Box& src_box,
                           const Box& dst_box) const {
  SDL_RenderCopy(sdl_renderer_, src.texture_, nullptr, nullptr);
}

void Renderer::Flip() const { SDL_RenderPresent(sdl_renderer_); }

void Renderer::ClearScreen() const { SDL_RenderClear(sdl_renderer_); }

}  // namespace troll
