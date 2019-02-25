#ifndef TROLL_SDL_TEXTURE_H_
#define TROLL_SDL_TEXTURE_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "proto/primitives.pb.h"

namespace troll {

class Renderer;

class Font {
 public:
  // Create a font from a file.
  static std::unique_ptr<Font> CreateFontFromFile(const std::string& filename,
                                                  int font_size);

  ~Font() { TTF_CloseFont(font_); }

  TTF_Font* font() const { return font_; }

 private:
  Font(TTF_Font* font) : font_(font) {}
  Font(const Font&) = delete;

  TTF_Font* font_;
};

class Texture {
 public:
  // Create a texture from an image file.
  static std::unique_ptr<Texture> CreateTextureFromFile(
      const std::string& filename, const RGBa& colour_key,
      const Renderer* renderer);

  // Create a texture with a text from a font asset.
  static std::unique_ptr<Texture> CreateTextureText(
      const std::string& text, const Font& font, const RGBa& colour,
      const RGBa& background_colour, const Renderer* renderer);

  // Create a texture of specified colour and dimensions.
  static std::unique_ptr<Texture> CreateTexture(const RGBa& colour, int width,
                                                int height);

  ~Texture() { SDL_DestroyTexture(texture_); }

  SDL_Texture* texture() const { return texture_; }

  Box GetBoundingBox() const;

 private:
  Texture(SDL_Texture* texture) : texture_(texture) {}
  Texture(const Texture&) = delete;

  SDL_Texture* texture_;
};

}  // namespace troll

#endif  // TROLL_SDL_TEXTURE_H_
