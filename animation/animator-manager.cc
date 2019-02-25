#include "animation/animator-manager.h"

#include <algorithm>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "core/event-dispatcher.h"
#include "core/events.h"

namespace troll {

namespace {
// Returns lambda that returns true if input script matches script id and
// scene node ids of this function.
auto MatchScriptSceneNode(const std::string& script_id,
                          const std::string& scene_node_id) {
  return [&](const std::unique_ptr<ScriptAnimator>& script) {
    return script->script_id() == script_id &&
           script->scene_node_id() == scene_node_id;
  };
}

// Returns lambda that returns true if input script matches scene node id of
// this function.
auto MatchSceneNode(const std::string& scene_node_id) {
  return [&](const std::unique_ptr<ScriptAnimator>& script) {
    return script->scene_node_id() == scene_node_id;
  };
}
}  // namespace

void AnimatorManager::Play(const AnimationScript& script,
                           const std::string& scene_node_id) {
  running_scripts_.push_back(
      std::make_unique<ScriptAnimator>(script, scene_node_id, core_));
  running_scripts_.back()->Start();
}

void AnimatorManager::Stop(const std::string& script_id,
                           const std::string& scene_node_id) const {
  for (const auto& script :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    script->Stop();
  }
}

void AnimatorManager::Pause(const std::string& script_id,
                            const std::string& scene_node_id) const {
  for (const auto& script :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    script->Pause();
  }
}

void AnimatorManager::Resume(const std::string& script_id,
                             const std::string& scene_node_id) const {
  for (const auto& script :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    script->Resume();
  }
}

void AnimatorManager::StopNodeAnimations(
    const std::string& scene_node_id) const {
  for (const auto& script :
       running_scripts_ | ranges::view::filter(MatchSceneNode(scene_node_id))) {
    script->Stop();
  }
}

void AnimatorManager::StopAll() { running_scripts_.clear(); }

void AnimatorManager::PauseAll() { paused_ = true; }

void AnimatorManager::ResumeAll() { paused_ = false; }

void AnimatorManager::Progress(int time_since_last_frame) {
  if (paused_) return;

  // Use index based iteration because new scripts might be added during
  // iteration as side effects.
  for (int i = 0; i < running_scripts_.size(); ++i) {
    running_scripts_[i]->Progress(time_since_last_frame);
  }

  // Clean up finished scripts and fire events.
  std::vector<std::string> events;
  const auto it = std::remove_if(
      running_scripts_.begin(), running_scripts_.end(),
      [this, &events](auto&& script) {
        if (script->is_finished()) {
          core_->event_dispatcher()->Emit(Events::OnAnimationScriptTermination(
              script->scene_node_id(), script->script_id()));
        }
        return script->is_finished();
      });
  running_scripts_.erase(it, running_scripts_.end());
}

}  // namespace troll
