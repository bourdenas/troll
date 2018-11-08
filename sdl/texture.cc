#include "sdl/texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>

#include "sdl/renderer.h"

namespace troll {

std::unique_ptr<Font> Font::CreateFontFromFile(const std::string& filename,
                                               int font_size) {
  TTF_Font* font = TTF_OpenFont(filename.c_str(), font_size);
  if (font == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  return std::unique_ptr<Font>(new Font(font));
}

std::unique_ptr<Texture> Texture::CreateTextureFromFile(
    const std::string& filename, const RGBa& colour_key) {
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (surface == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_SetColorKey(surface, SDL_TRUE,
                  SDL_MapRGB(surface->format, colour_key.red(),
                             colour_key.green(), colour_key.blue()));

  SDL_Texture* texture =
      SDL_CreateTextureFromSurface(Renderer::Instance().sdl_renderer_, surface);
  if (texture == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_FreeSurface(surface);

  return std::unique_ptr<Texture>(new Texture(texture));
}

std::unique_ptr<Texture> Texture::CreateTextureText(
    const std::string& text, const Font& font, const RGBa& colour,
    const RGBa& background_colour) {
  SDL_Surface* surface = TTF_RenderText_Blended(
      font.font(), text.c_str(),
      {static_cast<Uint8>(colour.red()), static_cast<Uint8>(colour.green()),
       static_cast<Uint8>(colour.blue()), static_cast<Uint8>(colour.alpha())});
  if (surface == nullptr) {
    LOG(ERROR) << SDL_GetError();
    return {};
  }

  SDL_Texture* texture =
      SDL_CreateTextureFromSurface(Renderer::Instance().sdl_renderer_, surface);
  if (texture == nullptr) {
    LOG(ERROR) << SDL_GetError();
  }
  SDL_FreeSurface(surface);

  return std::unique_ptr<Texture>(new Texture(texture));
}

std::unique_ptr<Texture> Texture::CreateTexture(const RGBa& colour, int width,
                                                int height) {
  return {};
}

Box Texture::GetBoundingBox() const {
  SDL_Rect rect;
  SDL_QueryTexture(texture_, NULL, NULL, &rect.w, &rect.h);

  Box bounding_box;
  bounding_box.set_width(rect.w);
  bounding_box.set_height(rect.h);
  return bounding_box;
}

}  // namespace troll
