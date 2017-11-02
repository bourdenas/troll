#ifndef TROLL_SDL_TEXTURE_H_
#define TROLL_SDL_TEXTURE_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "core/primitives.h"

namespace troll {

class Texture {
 public:
  Texture(SDL_Texture* texture) : texture_(texture) {}
  Texture(const Texture&) = delete;
  ~Texture() { SDL_DestroyTexture(texture_); }

  int GetPixel(const Point& at) { return 0; }
  void SetPixel(const Point& at, int pixel) {}

  Box GetBounds() const { return Box(); }
  void SetColourKey(const std::string& key) {}

  bool Overlap(Texture* right, const Box& intersection, const Box& leftRect,
               const Box& rightRect, unsigned leftKey, unsigned rightKey) {
    return false;
  }

  // private:
  SDL_Texture* texture_;
};

}  // namespace troll

#endif  // TROLL_SDL_TEXTURE_H_
