#include "animation/animator-manager.h"

#include <algorithm>

#include <range/v3/action/insert.hpp>
#include <range/v3/view/transform.hpp>

namespace troll {

void AnimatorManager::Init(const std::vector<SpriteAnimation>& animations) {
  for (const auto& animation : animations) {
    ranges::action::insert(
        scripts_, animation.script() | ranges::view::transform([](
                                           const AnimationScript& script) {
                    return std::make_pair(script.id(), script);
                  }));
  }
}

void AnimatorManager::Play(const std::string& script_id,
                           SceneNode* scene_node) {
  running_scripts_.push_back(
      std::make_unique<ScriptAnimator>(scripts_[script_id], scene_node));
  running_scripts_.back()->Start();
}

void AnimatorManager::PauseAll() { paused_ = true; }

void AnimatorManager::ResumeAll() { paused_ = false; }

void AnimatorManager::StopAll() { running_scripts_.clear(); }

void AnimatorManager::Progress(int time_since_last_frame) {
  if (paused_) return;

  const auto it =
      std::remove_if(running_scripts_.begin(), running_scripts_.end(),
                     [time_since_last_frame](auto& animator) {
                       animator->Progress(time_since_last_frame);
                       return animator->is_finished();
                     });
  running_scripts_.erase(it, running_scripts_.end());
}

}  // namespace troll
