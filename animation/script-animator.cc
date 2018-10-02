#include "animation/script-animator.h"

#include "core/scene-manager.h"
#include "core/troll-core.h"
#include "proto/scene-node.pb.h"

namespace troll {

void ScriptAnimator::Start() {
  auto* scene_node =
      Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
  if (scene_node != nullptr) {
    Core::Instance().scene_manager().Dirty(*scene_node);
  }

  if (!MoveToNextAnimation(scene_node)) {
    Stop();
    return;
  }

  state_ = State::RUNNING;
}

void ScriptAnimator::Stop() {
  const auto* scene_node =
      Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
  if (scene_node != nullptr) {
    Core::Instance().scene_manager().Dirty(*scene_node);
  }
  state_ = State::FINISHED;
}

void ScriptAnimator::Pause() { state_ = State::PAUSED; }

void ScriptAnimator::Resume() { state_ = State::RUNNING; }

void ScriptAnimator::Progress(int time_since_last_frame) {
  if (!is_running()) return;

  auto* scene_node =
      Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
  if (scene_node == nullptr) {
    Stop();
    return;
  }

  Core::Instance().scene_manager().Dirty(*scene_node);
  if (current_animator_->Progress(time_since_last_frame, scene_node) &&
      !MoveToNextAnimation(scene_node)) {
    Stop();
  }
}

bool ScriptAnimator::MoveToNextAnimation(SceneNode* scene_node) {
  if (scene_node == nullptr ||
      (next_animation_index_ == script_.animation().size() &&
       ++run_number_ == script_.repeat())) {
    return false;
  }

  if (next_animation_index_ == script_.animation().size()) {
    next_animation_index_ = 0;
  }

  current_animator_ = std::make_unique<Animator>();
  current_animator_->Start(script_.animation(next_animation_index_++),
                           scene_node);
  return true;
}

}  // namespace troll
