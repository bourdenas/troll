#ifndef TROLL_ANIMATION_ANIMATOR_MANAGER_H_
#define TROLL_ANIMATION_ANIMATOR_MANAGER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "animation/animator.h"
#include "proto/animation.pb.h"
#include "proto/scene.pb.h"

namespace troll {

class AnimatorManager {
 public:
  static AnimatorManager& Instance() {
    static AnimatorManager singleton;
    return singleton;
  }

  void Init(const std::vector<SpriteAnimation>& animations);

  void Play(const std::string& script_id, SceneNode* scene_node);

  void PauseAll();
  void ResumeAll();
  void StopAll();

  void Progress(int time_since_last_frame);

 private:
  AnimatorManager() = default;
  AnimatorManager(const AnimatorManager&) = delete;
  ~AnimatorManager() = default;

  void CheckAnimatorsStatus();
  void RemoveTerminated();

  std::unordered_map<std::string, AnimationScript> scripts_;

  bool paused_ = false;
  std::vector<std::unique_ptr<ScriptAnimator>> running_scripts_;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_ANIMATOR_MANAGER_H_
