#include "core/util-lib.h"

#include "core/resource-manager.h"
#include "proto/sprite.pb.h"

namespace troll {
namespace util {

Box GetSceneNodeBoundingBox(const SceneNode& node) {
  const auto* sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  auto bounding_box = sprite->film(node.frame_index());
  bounding_box.set_left(node.position().x());
  bounding_box.set_top(node.position().y());
  return bounding_box;
}

}  // namespace util
}  // namespace troll
