#include "sdl/texture.h"

namespace troll {

Box Texture::GetBoundingBox() const {
  SDL_Rect rect;
  SDL_QueryTexture(texture_, NULL, NULL, &rect.w, &rect.h);

  Box bounding_box;
  bounding_box.set_width(rect.w);
  bounding_box.set_height(rect.h);
  return bounding_box;
}

}  // namespace troll
