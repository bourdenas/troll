#include "core/util-lib.h"

#include "core/troll-core.h"
#include "proto/sprite.pb.h"

namespace troll {
namespace util {

Box GetSceneNodeBoundingBox(const SceneNode& node) {
  const auto* sprite = Core::Instance().GetSprite(node.sprite_id());
  if (sprite == nullptr) {
    return Box();
  }

  auto bounding_box = sprite->film(node.frame_index());
  bounding_box.set_left(node.position().x());
  bounding_box.set_top(node.position().y());
  return bounding_box;
}

}  // namespace util
}  // namespace troll
