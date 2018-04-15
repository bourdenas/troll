#include "core/collision-checker.h"

#include <glog/logging.h>
#include <range/v3/action/push_back.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"
#include "core/troll-core.h"
#include "core/util-lib.h"

namespace troll {

void CollisionChecker::Init() {
  collision_directory_.clear();
  dirty_nodes_.clear();
  colliding_node_pairs_.clear();
}

void CollisionChecker::RegisterCollision(const CollisionAction& collision) {
  collision_directory_.push_back(collision);
}

void CollisionChecker::Dirty(const SceneNode& node) {
  dirty_nodes_.insert(&node);
}

void CollisionChecker::CheckCollisions() {
  for (const auto* node : dirty_nodes_) {
    for (const auto& collision : collision_directory_) {
      CheckNodeCollision(*node, collision);
    }
  }
  dirty_nodes_.clear();
}

namespace {
// Returns true if the bounding boxes of two scene nodes are overlapping.
bool BoundingBoxesCollide(const SceneNode& left, const SceneNode& right) {
  const auto& scene = Core::Instance().scene_manager();
  const Box left_box = scene.GetSceneNodeBoundingBox(left);
  const Box right_box = scene.GetSceneNodeBoundingBox(right);
  return abs(left_box.left() - right_box.left()) * 2 <=
             left_box.width() + right_box.width() &&
         abs(left_box.top() - right_box.top()) * 2 <=
             left_box.height() + right_box.height();
}
}  // namespace

void CollisionChecker::CheckNodeCollision(const SceneNode& node,
                                          const CollisionAction& collision) {
  // Check if node is part of this collision definition.
  if (ranges::none_of(
          collision.scene_node_id(),
          [&node](const auto& node_id) { return node.id() == node_id; }) &&
      ranges::none_of(collision.sprite_id(), [&node](const auto& sprite_id) {
        return node.sprite_id() == sprite_id;
      })) {
    return;
  }

  // Collect all nodes in collision definition that need to be checked.
  std::vector<const SceneNode*> potential_colliding_nodes;
  potential_colliding_nodes |= ranges::action::push_back(
      collision.scene_node_id() | ranges::view::transform([](const auto& id) {
        return Core::Instance().scene_manager().GetSceneNodeById(id);
      }) |
      ranges::view::remove_if([&node](const SceneNode* other_node) {
        return other_node == nullptr || &node == other_node;
      }));
  for (const auto& sprite_id : collision.sprite_id()) {
    potential_colliding_nodes |= ranges::action::push_back(
        Core::Instance().scene_manager().GetSceneNodesBySprite(sprite_id) |
        ranges::view::transform([](const SceneNode& node) { return &node; }) |
        ranges::view::remove_if([&node](const SceneNode* other_node) {
          return &node == other_node;
        }));
  }

  // Check all node pairs for collision.
  if (ranges::none_of(potential_colliding_nodes,
                      [&node, this](const SceneNode* other_node) {
                        return NodePairTouched(node, *other_node);
                      })) {
    return;
  }

  for (const auto& action : collision.action()) {
    ActionManager::Instance().Execute(action);
  }
}

bool CollisionChecker::NodePairTouched(const SceneNode& left,
                                       const SceneNode& right) {
  const bool collide = BoundingBoxesCollide(left, right);
  if (NodePairAlreadyCollides(left, right)) {
    if (!collide) {
      RemoveCollidingNodePair(left, right);
    }
    return false;
  }

  if (collide) {
    AddCollidingNodePair(left, right);
  }
  return collide;
}

void CollisionChecker::AddCollidingNodePair(const SceneNode& left,
                                            const SceneNode& right) {
  if (&left < &right) {
    colliding_node_pairs_.emplace(&left, &right);
  } else {
    colliding_node_pairs_.emplace(&right, &left);
  }
}

void CollisionChecker::RemoveCollidingNodePair(const SceneNode& left,
                                               const SceneNode& right) {
  const auto pair = &left < &right ? std::make_pair(&left, &right)
                                   : std::make_pair(&right, &left);
  const auto it = colliding_node_pairs_.find(pair);
  if (it != colliding_node_pairs_.end()) {
    colliding_node_pairs_.erase(it);
  }
}

bool CollisionChecker::NodePairAlreadyCollides(const SceneNode& left,
                                               const SceneNode& right) {
  const auto pair = &left < &right ? std::make_pair(&left, &right)
                                   : std::make_pair(&right, &left);
  return colliding_node_pairs_.find(pair) != colliding_node_pairs_.end();
}

}  // namespace troll
