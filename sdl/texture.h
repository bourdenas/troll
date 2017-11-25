#ifndef TROLL_SDL_TEXTURE_H_
#define TROLL_SDL_TEXTURE_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "proto/primitives.pb.h"

namespace troll {

class Texture {
 public:
  Texture(SDL_Texture* texture) : texture_(texture) {}
  Texture(const Texture&) = delete;
  ~Texture() { SDL_DestroyTexture(texture_); }

  SDL_Texture* texture() const { return texture_; }

  Box GetBoundingBox() const;

 private:
  SDL_Texture* texture_;
};

class Font {
 public:
  Font(TTF_Font* font) : font_(font) {}
  Font(const Font&) = delete;
  ~Font() { TTF_CloseFont(font_); }

  TTF_Font* font() const { return font_; }

 private:
  TTF_Font* font_;
};

}  // namespace troll

#endif  // TROLL_SDL_TEXTURE_H_
