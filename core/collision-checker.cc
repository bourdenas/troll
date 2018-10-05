#include "core/collision-checker.h"

#include <absl/strings/str_join.h>
#include <glog/logging.h>
#include <range/v3/action/push_back.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"
#include "core/geometry.h"
#include "core/troll-core.h"

namespace troll {

void CollisionChecker::Init() {
  collision_directory_.clear();
  dirty_nodes_.clear();
  collision_cache_.clear();
}

void CollisionChecker::RegisterCollision(const CollisionAction& collision) {
  collision_directory_.push_back(collision);
}

void CollisionChecker::Dirty(const SceneNode& node) {
  dirty_nodes_.push_back(&node);
}

void CollisionChecker::CheckCollisions() {
  auto& scene_nodes = Core::Instance().scene_manager().GetSceneNodes();

  std::unordered_set<const SceneNode*> checked_nodes;
  for (int i = 0; i < dirty_nodes_.size(); ++i) {
    const auto& lhs = *dirty_nodes_[i];

    // Skip if scene node already checked for collisions during this frame.
    const auto result = checked_nodes.insert(&lhs);
    if (!result.second) continue;

    auto& aabb = Core::Instance().scene_manager().GetSceneNodeBoundingBox(lhs);

    for (const auto& rhs : scene_nodes) {
      // Skip if collision checking with self.
      if (&lhs == &rhs) continue;

      const bool collide = geo::Collide(
          aabb, Core::Instance().scene_manager().GetSceneNodeBoundingBox(rhs));

      if (NodesInCollisionCache(lhs, rhs)) {
        if (!collide) {
          RemoveFromCollisionCache(lhs, rhs);
        }
      } else if (collide) {
        AddInCollisionCache(lhs, rhs);
        TriggerCollision(lhs, rhs);
      }
    }
  }
  dirty_nodes_.clear();
}

void CollisionChecker::TriggerCollision(const SceneNode& lhs,
                                        const SceneNode& rhs) const {
  for (const auto& collision : collision_directory_) {
    if (!NodeInCollision(lhs, collision) || !NodeInCollision(rhs, collision)) {
      continue;
    }
    for (const auto& action : collision.action()) {
      ActionManager::Instance().Execute(action);
    }
  }
}

bool CollisionChecker::NodeInCollision(const SceneNode& node,
                                       const CollisionAction& collision) const {
  return ranges::any_of(collision.scene_node_id(),
                        [&node](const auto& id) { return node.id() == id; }) ||
         ranges::any_of(collision.sprite_id(), [&node](const auto& sprite_id) {
           return node.sprite_id() == sprite_id;
         });
}

void CollisionChecker::AddInCollisionCache(const SceneNode& left,
                                           const SceneNode& right) {
  if (&left < &right) {
    collision_cache_.emplace(&left, &right);
  } else {
    collision_cache_.emplace(&right, &left);
  }
}

void CollisionChecker::RemoveFromCollisionCache(const SceneNode& left,
                                                const SceneNode& right) {
  const auto pair = &left < &right ? std::make_pair(&left, &right)
                                   : std::make_pair(&right, &left);
  const auto it = collision_cache_.find(pair);
  if (it != collision_cache_.end()) {
    collision_cache_.erase(it);
  }
}

bool CollisionChecker::NodesInCollisionCache(const SceneNode& left,
                                             const SceneNode& right) const {
  const auto pair = &left < &right ? std::make_pair(&left, &right)
                                   : std::make_pair(&right, &left);
  return collision_cache_.find(pair) != collision_cache_.end();
}

}  // namespace troll
