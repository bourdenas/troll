#include "core/collision-checker.h"

#include <glog/logging.h>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"
#include "core/troll-core.h"
#include "core/util-lib.h"

namespace troll {

void CollisionChecker::Init() {
  dirty_nodes_.clear();
  node_collision_directory_.clear();
  sprite_collision_directory_.clear();
}

void CollisionChecker::RegisterCollision(const CollisionAction& collision) {
  LOG_IF(ERROR,
         (collision.sprite_id_size() + collision.scene_node_id_size()) < 2)
      << "Collision requires at least to sprites or scene nodes.\n"
      << collision.DebugString();

  ranges::action::insert(
      node_collision_directory_,
      collision.scene_node_id() | ranges::view::transform([&collision](
                                      const std::string& scene_node_id) {
        // Remove from collision the triggers criteria (node).
        CollisionAction collision_copy = collision;
        std::remove_if(
            collision_copy.mutable_scene_node_id()->begin(),
            collision_copy.mutable_scene_node_id()->end(),
            [&scene_node_id](const auto& id) { return id == scene_node_id; });
        collision_copy.mutable_scene_node_id()->RemoveLast();
        return std::make_pair(scene_node_id, collision_copy);
      }));

  ranges::action::insert(
      sprite_collision_directory_,
      collision.sprite_id() |
          ranges::view::transform([&collision](const std::string& sprite_id) {
            // Remove from collision the triggers criteria (sprite).
            CollisionAction collision_copy = collision;
            std::remove_if(
                collision_copy.mutable_sprite_id()->begin(),
                collision_copy.mutable_sprite_id()->end(),
                [&sprite_id](const auto& id) { return id == sprite_id; });
            collision_copy.mutable_sprite_id()->RemoveLast();
            return std::make_pair(sprite_id, collision_copy);
          }));
}

void CollisionChecker::Dirty(const SceneNode& node) {
  dirty_nodes_.insert(&node);
}

namespace {
// Returns true if the bounding boxes of two scene nodes are overlapping.
bool CheckBoundingBoxCollision(const SceneNode& left, const SceneNode& right) {
  const Box left_box = util::GetSceneNodeBoundingBox(left);
  const Box right_box = util::GetSceneNodeBoundingBox(right);
  return abs(left_box.left() - right_box.left()) * 2 <=
             left_box.width() + right_box.width() &&
         abs(left_box.top() - right_box.top()) * 2 <=
             left_box.height() + right_box.height();
}

void CheckNodeCollisions(const SceneNode& node,
                         const CollisionAction& collision) {
  if (!ranges::all_of(
          collision.scene_node_id() | ranges::view::transform([](
                                          const auto& node_id) {
            return Core::Instance().scene_manager().GetSceneNodeById(node_id);
          }),
          [&node](const SceneNode* other_node) {
            return &node != other_node &&
                   CheckBoundingBoxCollision(node, *other_node);
          })) {
    return;
  }

  for (const auto& sprite_id : collision.sprite_id()) {
    if (ranges::none_of(
            Core::Instance().scene_manager().GetSceneNodesBySprite(sprite_id),
            [&node](const SceneNode& other_node) {
              return &node != &other_node &&
                     CheckBoundingBoxCollision(node, other_node);
            })) {
      return;
    }
  }

  for (const auto& action : collision.action()) {
    ActionManager::Instance().Execute(action);
  }
}
}  // namespace

void CollisionChecker::CheckCollisions() {
  for (const auto* node : dirty_nodes_) {
    const auto node_range = node_collision_directory_.equal_range(node->id());
    for (auto it = node_range.first; it != node_range.second; ++it) {
      CheckNodeCollisions(*node, it->second);
    }

    const auto sprite_range =
        sprite_collision_directory_.equal_range(node->sprite_id());
    for (auto it = sprite_range.first; it != sprite_range.second; ++it) {
      CheckNodeCollisions(*node, it->second);
    }
  }
  dirty_nodes_.clear();
}

}  // namespace troll
