#ifndef TROLL_CORE_TEXTURE_H_
#define TROLL_CORE_TEXTURE_H_

#include <memory>
#include <string>

#include "core/primitives.h"

namespace troll {

class Texture {
 public:
  Texture() = default;
  Texture(const Texture&) = delete;
  virtual ~Texture() = default;

  virtual int GetPixel(const Point& at) = 0;
  virtual void SetPixel(const Point& at, int pixel) = 0;

  virtual const Box GetBounds() const = 0;
  virtual void SetColourKey(const std::string& key) = 0;

  virtual bool Overlap(Texture* right, const Box& intersection,
                       const Box& leftRect, const Box& rightRect,
                       unsigned leftKey, unsigned rightKey) = 0;
};

}  // namespace troll

#endif  // TROLL_CORE_TEXTURE_H_
