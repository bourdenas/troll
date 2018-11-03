#include "sdl/renderer.h"

#include <bitset>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>

#include "sdl/texture.h"

namespace troll {

namespace {
constexpr char* kResourcePath = "../data/resources/";
}  // namespace

void Renderer::Init(int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
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
  IMG_Quit();
  SDL_Quit();
}

std::vector<boost::dynamic_bitset<>> Renderer::GenerateCollisionMasks(
    const Sprite& sprite) const {
  SDL_Surface* sprite_surface =
      IMG_Load((kResourcePath + sprite.resource()).c_str());
  if (sprite_surface == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }

  SDL_Surface* surface = SDL_ConvertSurfaceFormat(
      sprite_surface, SDL_GetWindowPixelFormat(window_), NULL);
  SDL_FreeSurface(sprite_surface);

  const int bpp = surface->format->BytesPerPixel;
  LOG_IF(FATAL, bpp != 4)
      << "Surface format should be 4 bytes per pixel. Format is " << bpp
      << " bytes per pixels instead.";

  const Uint32 format = SDL_GetWindowPixelFormat(window_);
  SDL_PixelFormat* mapping_format = SDL_AllocFormat(format);
  const Uint32 colour_key =
      SDL_MapRGB(mapping_format, sprite.colour_key().red(),
                 sprite.colour_key().green(), sprite.colour_key().blue());
  SDL_FreeFormat(mapping_format);

  const int surface_width = surface->pitch / bpp;

  std::vector<boost::dynamic_bitset<>> collision_masks;
  SDL_LockSurface(surface);
  for (const auto& film : sprite.film()) {
    collision_masks.push_back(
        boost::dynamic_bitset<>(film.width() * film.height()));
    auto& collision_mask = collision_masks.back();

    const Uint32* pixels = static_cast<const Uint32*>(surface->pixels);
    for (int i = film.top(); i < film.top() + film.height(); ++i) {
      for (int j = film.left(); j < film.left() + film.width(); ++j) {
        const int surface_index = i * surface_width + j;
        const int mask_index =
            (i - film.top()) * film.width() + (j - film.left());
        collision_mask[mask_index] =
            pixels[surface_index] == colour_key ? 0 : 1;
      }
    }
  }
  SDL_UnlockSurface(surface);

  SDL_FreeSurface(surface);
  return collision_masks;
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
