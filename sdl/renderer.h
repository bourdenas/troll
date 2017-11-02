#ifndef TROLL_SDL_RENDERER_H_
#define TROLL_SDL_RENDERER_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "core/primitives.h"
#include "sdl/texture.h"

namespace troll {

class Renderer {
 public:
  void Init();
  void CleanUp();

  std::unique_ptr<Texture> CreateTexture(int width, int height,
                                         const RGBa& colour) {
    return nullptr;
  }
  std::unique_ptr<Texture> LoadTexture(const std::string& filename);

  void BlitTexture(const Texture& src, const Box& src_box,
                   const Box& dst_box) const;
  void Flip() const;
  void ClearScreen() const;

  void ShowCursor(bool show) {}
  Box CreateText(const std::string& id, const std::string& text,
                 const RGBa& fg_colour, const RGBa& bg_colour,
                 const std::string& font, size_t size, bool bold,
                 bool italics) {
    return Box();
  }
  void PrintText(const std::string& text, const Point& at) {}

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
};

}  // namespace troll

#endif  // TROLL_SDL_RENDERER_H_
