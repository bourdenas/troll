#ifndef TROLL_SDL_RENDERER_H_
#define TROLL_SDL_RENDERER_H_

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "proto/primitives.pb.h"
#include "sdl/texture.h"

namespace troll {

class Renderer {
 public:
  // Creates the rendering engine and window.
  void Init(int width, int height);
  void CleanUp();

  // Load an image file into a texture.
  std::unique_ptr<Texture> LoadTexture(const std::string& filename) const;
  std::unique_ptr<Font> LoadFont(const std::string& filename,
                                 int font_size) const;

  // Create a texture of the input size and colour.
  std::unique_ptr<Texture> CreateTexture(const RGBa& colour, int width,
                                         int height) const;
  // Create a text surface area.
  std::unique_ptr<Texture> CreateText(const std::string& text, const Font& font,
                                      const RGBa& colour,
                                      const RGBa& background_colour) const;

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
  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
};

}  // namespace troll

#endif  // TROLL_SDL_RENDERER_H_
