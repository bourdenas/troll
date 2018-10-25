#ifndef TROLL_CORE_COLLISION_CHECKER_H_
#define TROLL_CORE_COLLISION_CHECKER_H_

#include <set>
#include <string>

#include <boost/dynamic_bitset.hpp>

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
  void RegisterDetachment(const CollisionAction& detaching);

  // Add this node in the checking for collisions during this frame.
  void Dirty(const SceneNode& node);

  // Checks SceneNodes which were marked as dirty for collisions and applies
  // collision actions as consequences.
  void CheckCollisions();

 private:
  CollisionChecker() = default;
  CollisionChecker(const CollisionChecker&) = delete;
  ~CollisionChecker() = default;

  // Triggers actions associated with collision/detaching of input nodes.
  void TriggerCollisionAction(const SceneNode& lhs, const SceneNode& rhs) const;
  void TriggerDetachingAction(const SceneNode& lhs, const SceneNode& rhs) const;

  // Returns true if node is part of the CollisionAction description directly
  // (i.e. by scene_node_id) or indirectly (i.e. by sprite_id).
  bool NodeInCollision(const SceneNode& node,
                       const CollisionAction& collision) const;

  // Collision cache interface.
  void AddInCollisionCache(const SceneNode& left, const SceneNode& right);
  void RemoveFromCollisionCache(const SceneNode& left, const SceneNode& right);
  bool NodesInCollisionCache(const SceneNode& left,
                             const SceneNode& right) const;

  // Directory of registered collisions.
  std::vector<CollisionAction> collision_directory_;

  // Directory of registered detachments.
  std::vector<CollisionAction> detachment_directory_;

  // Nodes that moved or created during this frame and should be checked for
  // collisions.
  std::vector<const SceneNode*> dirty_nodes_;

  // Collision cache to remember what nodes were already colliding before this
  // frame started.
  std::set<std::pair<const SceneNode*, const SceneNode*>> collision_cache_;
};

namespace internal {
// Returns true if the input bounding boxes actaully collide based on supplied
// collision masks.
bool SceneNodePixelsCollide(const Box& lhs_aabb, const Box& rhs_aabb,
                            const boost::dynamic_bitset<>& lhs_collision_mask,
                            const boost::dynamic_bitset<>& rhs_collision_mask);
}  // namespace internal

}  // namespace troll

#endif  // TROLL_CORE_COLLISION_CHECKER_H_
