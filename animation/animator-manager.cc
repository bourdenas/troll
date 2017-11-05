#include "animation/animator-manager.h"

#include <algorithm>

namespace troll {

void AnimatorManager::Register(const AnimationScript& script,
                               SceneNode* scene_node) {
  scripts_.push_back(std::make_unique<ScriptAnimator>(script, scene_node));
}

void AnimatorManager::Clear() { scripts_.clear(); }

void AnimatorManager::Progress(int time_since_last_frame) {
  const auto it = std::remove_if(scripts_.begin(), scripts_.end(),
                                 [time_since_last_frame](auto& animator) {
                                   animator->Progress(time_since_last_frame);
                                   return animator->is_finished();
                                 });
  scripts_.erase(it, scripts_.end());
}

}  // namespace troll
