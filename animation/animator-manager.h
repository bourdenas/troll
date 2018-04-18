#ifndef TROLL_ANIMATION_ANIMATOR_MANAGER_H_
#define TROLL_ANIMATION_ANIMATOR_MANAGER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "animation/script-animator.h"
#include "proto/animation.pb.h"
#include "proto/scene.pb.h"

namespace troll {

class AnimatorManager {
 public:
  static AnimatorManager& Instance() {
    static AnimatorManager singleton;
    return singleton;
  }

  void Play(const std::string& script_id, const std::string& scene_node_id);
  void Stop(const std::string& script_id,
            const std::string& scene_node_id) const;
  void Pause(const std::string& script_id,
             const std::string& scene_node_id) const;
  void Resume(const std::string& script_id,
              const std::string& scene_node_id) const;

  void StopNodeAnimations(const std::string& scene_node_id) const;

  void StopAll();
  void PauseAll();
  void ResumeAll();

  void Progress(int time_since_last_frame);

  AnimatorManager(const AnimatorManager&) = delete;
  AnimatorManager& operator=(const AnimatorManager&) = delete;

 private:
  AnimatorManager() = default;
  ~AnimatorManager() = default;

  bool paused_ = false;
  std::vector<std::unique_ptr<ScriptAnimator>> running_scripts_;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_ANIMATOR_MANAGER_H_
