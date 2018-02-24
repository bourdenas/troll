#ifndef TROLL_CORE_COLLISION_CHECKER_H_
#define TROLL_CORE_COLLISION_CHECKER_H_

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "proto/action.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {

class CollisionChecker {
 public:
  static CollisionChecker& Instance() {
    static CollisionChecker singleton;
    return singleton;
  }

  void Init();
  void RegisterCollision(const CollisionAction& collision);

  // Marks a scene node that needs to be queued for rendering during this frame.
  void Dirty(const SceneNode& node);

  // Checks SceneNodes which were marked as dirty for collisions and applies
  // collision actions as consequences.
  void CheckCollisions();

 private:
  CollisionChecker() = default;
  CollisionChecker(const CollisionChecker&) = delete;
  ~CollisionChecker() = default;

  void CheckNodeCollisions(const SceneNode& node,
                           const CollisionAction& collision);

  void AddCollidingNodes(const SceneNode& left, const SceneNode& right);
  void RemoveCollidingNodes(const SceneNode& left, const SceneNode& right);
  bool NodesAlreadyCollide(const SceneNode& left, const SceneNode& right);

  // Directories of scene_node_id or sprite_id to Collision. Multiple copies of
  // collisions exist in memory, indexed individually by each of the involved
  // object for more efficient lookup.
  std::unordered_multimap<std::string, CollisionAction>
      node_collision_directory_;
  std::unordered_multimap<std::string, CollisionAction>
      sprite_collision_directory_;

  // Nodes that moved during this frame and should be checked for collisions.
  std::unordered_set<const SceneNode*> dirty_nodes_;

  // Node pairs that are in collision.
  std::set<std::pair<const SceneNode*, const SceneNode*>> colliding_nodes_;
};

}  // namespace troll

#endif  // TROLL_CORE_COLLISION_CHECKER_H_
