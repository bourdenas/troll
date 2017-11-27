#ifndef TROLL_CORE_COLLISION_CHECKER_H_
#define TROLL_CORE_COLLISION_CHECKER_H_

#include <map>
#include <string>
#include <tuple>
#include <unordered_set>

#include "proto/scene-node.pb.h"
#include "proto/scene.pb.h"

namespace troll {

class CollisionChecker {
 public:
  static CollisionChecker& Instance() {
    static CollisionChecker singleton;
    return singleton;
  }

  void Init(const Scene& scene);
  void CleanUp();

  void AddSceneNode(const SceneNode& scene_node);
  void RemoveSceneNode(const std::string& node_id);

  // Marks a scene node that needs to be queued for rendering during this frame.
  void Dirty(const SceneNode& node);

  // Checks SceneNodes for collisions and applies attached collision actions.
  void CheckCollisions();

 private:
  CollisionChecker() = default;
  CollisionChecker(const CollisionChecker&) = delete;
  ~CollisionChecker() = default;

  // Directory of unordered SceneNode pairs to Collision definition.
  std::map<std::tuple<std::string, std::string>, Collision>
      collision_directory_;

  // Active SceneNodes being tracked for collisions.
  std::vector<const SceneNode*> active_nodes_;

  // Nodes that moved during this frame and should be checked for collisions.
  std::unordered_set<const SceneNode*> dirty_nodes_;
};

}  // namespace troll

#endif  // TROLL_CORE_COLLISION_CHECKER_H_
