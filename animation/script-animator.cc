#include "animation/script-animator.h"

#include "core/scene-manager.h"
#include "core/troll-core.h"
#include "proto/scene-node.pb.h"

namespace troll {

void ScriptAnimator::Start() {
  if (!MoveToNextAnimation()) return;

  state_ = State::RUNNING;
  const auto* scene_node =
      Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
  Core::Instance().scene_manager().Dirty(*scene_node);
}

void ScriptAnimator::Stop() { state_ = State::FINISHED; }

void ScriptAnimator::Pause() { state_ = State::PAUSED; }

void ScriptAnimator::Resume() { state_ = State::RUNNING; }

bool ScriptAnimator::Progress(int time_since_last_frame) {
  if (!is_running()) return true;

  auto* scene_node =
      Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
  if (scene_node == nullptr) {
    Stop();
    return is_finished();
  }

  Core::Instance().scene_manager().Dirty(*scene_node);
  if (current_animator_->Progress(time_since_last_frame, scene_node) &&
      !MoveToNextAnimation()) {
    Stop();
  }
  return is_finished();
}

bool ScriptAnimator::MoveToNextAnimation() {
  if (next_animation_index_ == script_.animation().size()) return false;

  current_animator_ =
      std::make_unique<Animator>(script_.animation(next_animation_index_++));
  current_animator_->Start();
  return true;
}

}  // namespace troll
