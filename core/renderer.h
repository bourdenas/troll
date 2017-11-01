#ifndef TROLL_CORE_RENDERER_H_
#define TROLL_CORE_RENDERER_H_

#include <memory>
#include <string>

#include "core/bitmap.h"
#include "core/primitives.h"

namespace troll {

class Renderer {
 public:
  virtual void Init() = 0;
  virtual void CleanUp() = 0;

  // Bitmap API
  virtual Bitmap* CreateBitmap(int width, int height, const RGBa& colour) = 0;
  virtual Bitmap* LoadBitmap(const std::string& filename) = 0;

  virtual void BlitBitmap(const Box& src_box, const Box& dst_box,
                          const Bitmap& src, Bitmap* dst) = 0;

  virtual void ShowCursor(bool show) = 0;
  virtual Box CreateText(const std::string& id, const std::string& text,
                         const RGBa& fg_colour, const RGBa& bg_colour,
                         const std::string& font, size_t size, bool bold,
                         bool italics) = 0;
  virtual void PrintText(const std::string& text, const Point& at) = 0;

  virtual void Flip() = 0;
  virtual void ClearScreen() = 0;

  const Bitmap& bitmap() const { return bitmap_; }

 protected:
  ///--- class Renderer protected members
  std::unique_ptr<Bitmap> bitmap_;
};

}  // namespace troll

#endif  // TROLL_CORE_RENDERER_H_
