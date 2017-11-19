#include "animation/animator.h"

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/transform.hpp>

#include "core/scene-manager.h"
#include "core/troll-core.h"

namespace troll {

void Animator::Start() {
  if (animation_.has_translation()) {
    executors_.push_back(
        std::make_unique<TranslationExecutor>(animation_.translation()));
  }
  if (animation_.has_rotation()) {
    executors_.push_back(
        std::make_unique<RotationExecutor>(animation_.rotation()));
  }
  if (animation_.has_scaling()) {
    executors_.push_back(
        std::make_unique<ScalingExecutor>(animation_.scaling()));
  }
  if (animation_.has_frame_range()) {
    executors_.push_back(
        std::make_unique<FrameRangeExecutor>(animation_.frame_range()));
  }
  if (animation_.has_frame_list()) {
    executors_.push_back(
        std::make_unique<FrameListExecutor>(animation_.frame_list()));
  }
  if (animation_.has_go_to()) {
    executors_.push_back(std::make_unique<GotoExecutor>(animation_.go_to()));
  }
  if (animation_.has_flash()) {
    executors_.push_back(std::make_unique<FlashExecutor>(animation_.flash()));
  }
}

bool Animator::Progress(int time_since_last_frame, SceneNode* scene_node) {
  const bool is_done = ranges::accumulate(
      executors_ |
          ranges::view::transform(
              [time_since_last_frame,
               scene_node](const std::unique_ptr<Executor>& executor) -> bool {
                return executor->Progress(time_since_last_frame, scene_node);
              }),
      false, std::logical_or<bool>());

  return !(is_done && ++run_number_ == animation_.repeat());
}

void ScriptAnimator::Start() {
  if (MoveToNextAnimation()) {
    state_ = State::RUNNING;
  }
}

void ScriptAnimator::Stop() { state_ = State::FINISHED; }

void ScriptAnimator::Pause() { state_ = State::PAUSED; }

void ScriptAnimator::Resume() { state_ = State::RUNNING; }

bool ScriptAnimator::Progress(int time_since_last_frame) {
  if (!is_running()) return true;

  Core::Instance().scene_manager().Dirty(scene_node_);
  if (!current_animator_->Progress(time_since_last_frame, scene_node_) &&
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
