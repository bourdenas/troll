#include "core/collision-checker.h"

#include <glog/logging.h>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include "core/action-manager.h"
#include "core/util-lib.h"

namespace troll {

namespace {

std::tuple<std::string, std::string> CollisionDirectoryKey(
    const std::string& node_a, const std::string& node_b) {
  const auto& left = node_a < node_b ? node_a : node_b;
  const auto& right = node_a < node_b ? node_b : node_a;
  return std::make_tuple(left, right);
}

}  // namespace

void CollisionChecker::Init(const Scene& scene) {
  collision_directory_ =
      scene.collision() |
      ranges::view::remove_if([](const Collision& collision) {
        if (collision.sprite_id_size() != 2) {
          LOG(ERROR) << "Ignoring collision defining "
                     << collision.sprite_id_size() << " colliding sprites:\n"
                     << collision.DebugString();
          return true;
        }
        return false;
      }) |
      ranges::view::transform([](const Collision& collision) {
        return std::make_pair(CollisionDirectoryKey(collision.sprite_id(0),
                                                    collision.sprite_id(1)),
                              collision);
      });
}

void CollisionChecker::CleanUp() {
  dirty_nodes_.clear();
  active_nodes_.clear();
  collision_directory_.clear();
}

void CollisionChecker::AddSceneNode(const SceneNode& scene_node) {
  active_nodes_.push_back(&scene_node);
  dirty_nodes_.insert(&scene_node);
}

void CollisionChecker::RemoveSceneNode(const SceneNode& scene_node) {
  active_nodes_.erase(
      std::remove(active_nodes_.begin(), active_nodes_.end(), &scene_node));
}

void CollisionChecker::Dirty(const SceneNode& node) {
  dirty_nodes_.insert(&node);
}

bool CheckCollision(const SceneNode& left, const SceneNode& right) {
  const Box left_box = util::GetSceneNodeBoundingBox(left);
  const Box right_box = util::GetSceneNodeBoundingBox(right);
  return abs(left_box.left() - right_box.left()) * 2 <=
             left_box.width() + right_box.width() &&
         abs(left_box.top() - right_box.top()) * 2 <=
             left_box.height() + right_box.height();
}

void CollisionChecker::CheckCollisions() {
  for (const auto* node : dirty_nodes_) {
    for (const auto* other : active_nodes_) {
      if (node == other || !CheckCollision(*node, *other)) continue;

      const auto it = collision_directory_.find(
          CollisionDirectoryKey(node->sprite_id(), other->sprite_id()));
      if (it == collision_directory_.end()) continue;

      for (const auto& action : it->second.action()) {
        ActionManager::Instance().Execute(action);
      }
    }
  }
  dirty_nodes_.clear();
}

}  // namespace troll
