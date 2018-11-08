#ifndef TROLL_SDL_RENDERER_H_
#define TROLL_SDL_RENDERER_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>
#include <boost/dynamic_bitset.hpp>

#include "proto/primitives.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/texture.h"

namespace troll {

class Renderer {
 public:
  static Renderer& Instance() {
    static Renderer singleton;
    return singleton;
  }

  void Init(int width, int height);
  void CleanUp();

  // Returns collision masks for each film in the sprite. Masks are auto-
  // generated from the sprite's image and colour key
  std::vector<boost::dynamic_bitset<>> GenerateCollisionMasks(
      const std::string& base_path, const Sprite& sprite) const;

  // Blit a texture area to the screen.
  void BlitTexture(const Texture& src, const Box& src_box,
                   const Box& dst_box) const;
  // Fill the destination area with specified colour.
  void FillColour(const RGBa& colour, const Box& dst_box) const;

  // Commit all changes to the screen.
  void Flip() const;

  void ShowCursor(bool show) const {}

  void PrintText(const std::string& text, const Vector& at) const {}
  void ClearScreen() const;

 private:
  Renderer() = default;
  ~Renderer() = default;

  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;

  friend class Texture;
};

}  // namespace troll

#endif  // TROLL_SDL_RENDERER_H_
