#include "core/collision-checker.h"

#include <unordered_set>

#include <range/v3/action/sort.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/unique.hpp>

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

namespace {
std::pair<const SceneNode*, const SceneNode*> MakeOrderedPair(
    const SceneNode* left, const SceneNode* right) {
  return (left < right) ? std::make_pair(left, right)
                        : std::make_pair(right, left);
}
}  // namespace

void CollisionChecker::CheckCollisions() {
  const auto& scene_nodes = scene_manager_->GetSceneNodes();
  std::set<std::pair<const SceneNode*, const SceneNode*>> collision_pairs;
  std::set<std::pair<const SceneNode*, const SceneNode*>> detach_pairs;

  dirty_nodes_ |= ranges::action::sort;
  for (const auto& lhs :
       dirty_nodes_ | ranges::view::unique | ranges::view::indirect) {
    const auto lhs_aabb = scene_manager_->GetSceneNodeBoundingBox(lhs);

    for (const auto& rhs : scene_nodes) {
      // Skip if collision checking with self.
      if (&lhs == &rhs) continue;

      const auto rhs_aabb = scene_manager_->GetSceneNodeBoundingBox(rhs);

      if (!geo::Collide(lhs_aabb, rhs_aabb)) {
        const auto pair = MakeOrderedPair(&lhs, &rhs);
        if (collision_cache_.find(pair) != collision_cache_.end()) {
          detach_pairs.insert(pair);
        }
        continue;
      }

      const auto& lhs_mask = core_->resource_manager()->GetSpriteCollisionMask(
          lhs.sprite_id(), lhs.frame_index());
      const auto& rhs_mask = core_->resource_manager()->GetSpriteCollisionMask(
          rhs.sprite_id(), rhs.frame_index());
      bool collision = internal::SceneNodePixelsCollide(lhs_aabb, rhs_aabb,
                                                        lhs_mask, rhs_mask);

      if (!collision) {
        const auto pair = MakeOrderedPair(&lhs, &rhs);
        if (collision_cache_.find(pair) != collision_cache_.end()) {
          detach_pairs.insert(pair);
        }
        continue;
      }

      collision_pairs.insert(MakeOrderedPair(&lhs, &rhs));
    }
  }
  dirty_nodes_.clear();

  for (const auto [lhs, rhs] : detach_pairs) {
    collision_cache_.erase(MakeOrderedPair(lhs, rhs));
    TriggerCollisionAction(*lhs, *rhs, detachment_directory_);
  }

  for (const auto [lhs, rhs] : collision_pairs) {
    const auto pair = MakeOrderedPair(lhs, rhs);
    if (collision_cache_.find(pair) == collision_cache_.end()) {
      collision_cache_.insert(pair);
      TriggerCollisionAction(*lhs, *rhs, collision_directory_);
    }

    TriggerCollisionAction(*lhs, *rhs, overlap_directory_);
  }
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
