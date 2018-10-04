#include "sdl/renderer.h"

#include <glog/logging.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "sdl/texture.h"

namespace troll {

void Renderer::Init(int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    LOG(ERROR) << "SDL_Init error: " << SDL_GetError();
    return;
  }

  if (TTF_Init() != 0) {
    LOG(ERROR) << "TTF_Init: " << SDL_GetError();
    return;
  }

  window_ = SDL_CreateWindow("Troll the World!", 100, 100, width, height,
                             SDL_WINDOW_SHOWN);
  if (window_ == nullptr) {
    LOG(ERROR) << "SDL_CreateWindow Error: " << SDL_GetError();
    return;
  }

  sdl_renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (sdl_renderer_ == nullptr) {
    LOG(ERROR) << "SDL_CreateRenderer Error: " << SDL_GetError();
    return;
  }
}

void Renderer::CleanUp() {
  SDL_DestroyRenderer(sdl_renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

std::unique_ptr<Texture> Renderer::LoadTexture(const std::string& filename,
                                               const RGBa& colour_key) const {
  constexpr char* kResourcePath = "../data/resources/";
  SDL_Surface* surface = IMG_Load((kResourcePath + filename).c_str());
  if (surface == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_SetColorKey(surface, SDL_TRUE,
                  SDL_MapRGB(surface->format, colour_key.red(),
                             colour_key.green(), colour_key.blue()));

  SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer_, surface);
  if (texture == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_FreeSurface(surface);

  return std::make_unique<Texture>(texture);
}

std::unique_ptr<Font> Renderer::LoadFont(const std::string& filename,
                                         int font_size) const {
  constexpr char* kResourcePath = "../data/resources/";

  TTF_Font* font = TTF_OpenFont((kResourcePath + filename).c_str(), font_size);
  if (font == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  return std::make_unique<Font>(font);
}

std::unique_ptr<Texture> Renderer::CreateTexture(const RGBa& colour, int width,
                                                 int height) const {
  return nullptr;
}

std::unique_ptr<Texture> Renderer::CreateText(
    const std::string& text, const Font& font, const RGBa& colour,
    const RGBa& background_colour) const {
  SDL_Surface* surface = TTF_RenderText_Blended(
      font.font(), text.c_str(),
      {static_cast<Uint8>(colour.red()), static_cast<Uint8>(colour.green()),
       static_cast<Uint8>(colour.blue()), static_cast<Uint8>(colour.alpha())});
  if (surface == nullptr) {
    LOG(ERROR) << SDL_GetError();
    return {};
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer_, surface);
  if (texture == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_FreeSurface(surface);

  return std::make_unique<Texture>(texture);
}

void Renderer::BlitTexture(const Texture& src, const Box& src_box,
                           const Box& dst_box) const {
  SDL_Rect src_rect = {
      src_box.left(), src_box.top(), src_box.width(), src_box.height(),
  };
  SDL_Rect dst_rect = {
      dst_box.left(), dst_box.top(), dst_box.width(), dst_box.height(),
  };
  SDL_RenderCopy(sdl_renderer_, src.texture(),
                 src_box.width() == 0 ? nullptr : &src_rect,
                 dst_box.width() == 0 ? nullptr : &dst_rect);
}

void Renderer::FillColour(const RGBa& colour, const Box& dst_box) const {
  SDL_SetRenderDrawColor(sdl_renderer_, colour.red(), colour.green(),
                         colour.blue(), colour.alpha());
  SDL_Rect dst_rect = {
      dst_box.left(), dst_box.top(), dst_box.width(), dst_box.height(),
  };
  SDL_RenderFillRect(sdl_renderer_, &dst_rect);
}

void Renderer::Flip() const { SDL_RenderPresent(sdl_renderer_); }

void Renderer::ClearScreen() const { SDL_RenderClear(sdl_renderer_); }

}  // namespace troll
