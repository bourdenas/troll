#include "animation/animator-manager.h"

#include <algorithm>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "core/resource-manager.h"

namespace troll {

namespace {
auto MatchScriptSceneNode(const std::string& script_id,
                          const std::string& scene_node_id) {
  return [&](const std::unique_ptr<ScriptAnimator>& animator) {
    return animator->script_id() == script_id &&
           animator->scene_node_id() == scene_node_id;
  };
}

auto MatchSceneNode(const std::string& scene_node_id) {
  return [&](const std::unique_ptr<ScriptAnimator>& animator) {
    return animator->scene_node_id() == scene_node_id;
  };
}
}  // namespace

void AnimatorManager::Play(const std::string& script_id,
                           const std::string& scene_node_id) {
  running_scripts_.push_back(std::make_unique<ScriptAnimator>(
      ResourceManager::Instance().GetAnimationScript(script_id),
      scene_node_id));
  running_scripts_.back()->Start();
}

void AnimatorManager::Stop(const std::string& script_id,
                           const std::string& scene_node_id) {
  for (const auto& animator :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    animator->Stop();
  }
}

void AnimatorManager::Pause(const std::string& script_id,
                            const std::string& scene_node_id) {
  for (const auto& animator :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    animator->Pause();
  }
}

void AnimatorManager::Resume(const std::string& script_id,
                             const std::string& scene_node_id) {
  for (const auto& animator :
       running_scripts_ | ranges::view::filter(
                              MatchScriptSceneNode(script_id, scene_node_id))) {
    animator->Resume();
  }
}

void AnimatorManager::StopNodeAnimations(const std::string& scene_node_id) {
  for (auto& animator :
       running_scripts_ | ranges::view::filter(MatchSceneNode(scene_node_id))) {
    animator->Stop();
  }
}

void AnimatorManager::StopAll() { running_scripts_.clear(); }

void AnimatorManager::PauseAll() { paused_ = true; }

void AnimatorManager::ResumeAll() { paused_ = false; }

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
