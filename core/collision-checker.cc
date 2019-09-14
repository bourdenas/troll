#include "core/collision-checker.h"

#include <unordered_set>

#include <range/v3/algorithm/any_of.hpp>

#include "action/action-manager.h"
#include "core/geometry.h"
#include "core/resource-manager.h"

namespace troll {

void CollisionChecker::RegisterCollision(const CollisionAction& collision) {
  collision_directory_.push_back(collision);
}

void CollisionChecker::RegisterOverlap(const CollisionAction& overlap) {
  overlap_directory_.push_back(overlap);
}

void CollisionChecker::RegisterDetachment(const CollisionAction& detaching) {
  detachment_directory_.push_back(detaching);
}

void CollisionChecker::Dirty(const SceneNode& node) {
  dirty_nodes_.push_back(&node);
}

void CollisionChecker::CheckCollisions() {
  const auto& scene_nodes = scene_manager_->GetSceneNodes();

  std::unordered_set<const SceneNode*> checked_nodes;
  for (int i = 0; i < dirty_nodes_.size(); ++i) {
    const auto& lhs = *dirty_nodes_[i];

    // Skip if scene node already checked for collisions during this frame.
    const auto [it, added] = checked_nodes.insert(&lhs);
    if (!added) continue;

    const auto& lhs_aabb = scene_manager_->GetSceneNodeBoundingBox(lhs);
    for (const auto& rhs : scene_nodes) {
      // Skip if collision checking with self.
      if (&lhs == &rhs) continue;

      const auto& rhs_aabb = scene_manager_->GetSceneNodeBoundingBox(rhs);
      bool collision = false;
      if (geo::Collide(lhs_aabb, rhs_aabb)) {
        const auto& lhs_mask =
            core_->resource_manager()->GetSpriteCollisionMask(
                lhs.sprite_id(), lhs.frame_index());
        const auto& rhs_mask =
            core_->resource_manager()->GetSpriteCollisionMask(
                rhs.sprite_id(), rhs.frame_index());

        collision = internal::SceneNodePixelsCollide(lhs_aabb, rhs_aabb,
                                                     lhs_mask, rhs_mask);
      }

      if (NodesInCollisionCache(lhs, rhs)) {
        if (collision) {
          TriggerCollisionAction(lhs, rhs, overlap_directory_);
        } else {
          RemoveFromCollisionCache(lhs, rhs);
          TriggerCollisionAction(lhs, rhs, detachment_directory_);
        }
      } else if (collision) {
        AddInCollisionCache(lhs, rhs);
        TriggerCollisionAction(lhs, rhs, collision_directory_);
        TriggerCollisionAction(lhs, rhs, overlap_directory_);
      }
    }
  }
  dirty_nodes_.clear();
}

std::vector<std::string> CollisionChecker::collision_context() const {
  return !collision_context_.empty() ? collision_context_.top().nodes()
                                     : std::vector<std::string>();
}

void CollisionChecker::TriggerCollisionAction(
    const SceneNode& lhs, const SceneNode& rhs,
    const std::vector<CollisionAction>& collision_directory) {
  collision_context_.push(CollisionContext({lhs.id(), rhs.id()}));
  for (const auto& collision : collision_directory) {
    if (!NodeInCollision(lhs, collision) || !NodeInCollision(rhs, collision)) {
      continue;
    }
    for (const auto& action : collision.action()) {
      action_manager_->Execute(action);
    }
  }
  collision_context_.pop();
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

namespace internal {
bool SceneNodePixelsCollide(const Box& lhs_aabb, const Box& rhs_aabb,
                            const std::vector<bool>& lhs_collision_mask,
                            const std::vector<bool>& rhs_collision_mask) {
  const auto intersection = geo::Intersection(lhs_aabb, rhs_aabb);
  const int lhs_rel_top = intersection.top() - lhs_aabb.top();
  const int lhs_rel_left = intersection.left() - lhs_aabb.left();
  const int rhs_rel_top = intersection.top() - rhs_aabb.top();
  const int rhs_rel_left = intersection.left() - rhs_aabb.left();

  for (int y = 0; y < intersection.height(); ++y) {
    for (int x = 0; x < intersection.width(); ++x) {
      const int lhs_mask_index =
          (lhs_rel_top + y) * lhs_aabb.width() + (lhs_rel_left + x);
      const int rhs_mask_index =
          (rhs_rel_top + y) * rhs_aabb.width() + (rhs_rel_left + x);

      if (lhs_collision_mask[lhs_mask_index] == 1 &&
          rhs_collision_mask[rhs_mask_index] == 1) {
        return true;
      }
    }
  }
  return false;
}
}  // namespace internal

}  // namespace troll
