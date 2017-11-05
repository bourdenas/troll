#ifndef TROLL_ANIMATION_ANIMATOR_MANAGER_H_
#define TROLL_ANIMATION_ANIMATOR_MANAGER_H_

#include <memory>
#include <vector>

#include "animation/animator.h"
#include "proto/animation.pb.h"

namespace troll {

class AnimatorManager {
 public:
  static AnimatorManager& Instance() {
    static AnimatorManager singleton;
    return singleton;
  }

  void Register(const AnimationScript& script, SceneNode* scene_node);
  void Clear();

  void Progress(int time_since_last_frame);

 private:
  AnimatorManager() = default;
  AnimatorManager(const AnimatorManager&) = delete;
  ~AnimatorManager() = default;

  void CheckAnimatorsStatus();
  void RemoveTerminated();

  std::vector<std::unique_ptr<ScriptAnimator>> scripts_;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_ANIMATOR_MANAGER_H_
