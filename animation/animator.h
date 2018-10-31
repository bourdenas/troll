#ifndef TROLL_ANIMATION_ANIMATOR_H_
#define TROLL_ANIMATION_ANIMATOR_H_

#include <memory>
#include <vector>

#include "animation/performer.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {

// Runs a composite animation containing different types of transformations on a
// scene node. See also, "proto/animation.proto".
class Animator {
 public:
  // Initialises the animator state.
  void Start(const Animation& animation, SceneNode* scene_node);

  // Stops all performers of this animation.
  void Stop(const SceneNode& scene_node);

  // Returns true if the Animator finished.
  bool Progress(int time_since_last_frame, SceneNode* scene_node);

 private:
  // Returns true if any of the performers finished during progress.
  bool ProgressAny(int time_since_last_frame, SceneNode* scene_node);

  // Returns true iff all the performers finished during progress.
  bool ProgressAll(int time_since_last_frame, SceneNode* scene_node);

  // If true, the animator finishes when all performers are finished. Otherwise,
  // the animator is finished when any performer is finished.
  bool wait_for_all_ = false;

  std::vector<std::unique_ptr<Performer>> performers_;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_ANIMATOR_H_
