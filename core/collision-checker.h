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

  // Checks is a node is affected by collision definition.
  void CheckNodeCollision(const SceneNode& node,
                          const CollisionAction& collision);

  // Returns true if the node pair touched in this frame while they were not
  // touching before.
  bool NodePairTouched(const SceneNode& left, const SceneNode& right);

  // Functions for keeping track of colliding nodes to avoid triggering
  // collision constantly. The intent is to trigger the collision only when the
  // event happens.
  void AddCollidingNodePair(const SceneNode& left, const SceneNode& right);
  void RemoveCollidingNodePair(const SceneNode& left, const SceneNode& right);
  bool NodePairAlreadyCollides(const SceneNode& left, const SceneNode& right);

  // Directory of registered collisions.
  std::vector<CollisionAction> collision_directory_;

  // Nodes that moved during this frame and should be checked for collisions.
  std::unordered_set<const SceneNode*> dirty_nodes_;

  // Node pairs that are in collision.
  //
  // NB: It is possible that ptrs in the set might end up to be dangling ptrs.
  // For instance, a SceneNode is destroyed in collision. There is not problem
  // of dereferencing these pointers. They are only used for identifying
  // SceneNodes (like fingerprints). One possible bug could be, that the same
  // address gets reallocated to a different node. This could prevent the new
  // pair to collide. This might be a bit far-fetched but possible. Also it
  // increases unnecessarily the size of the set if many nodes get destroyed.
  std::set<std::pair<const SceneNode*, const SceneNode*>> colliding_node_pairs_;
};

}  // namespace troll

#endif  // TROLL_CORE_COLLISION_CHECKER_H_
