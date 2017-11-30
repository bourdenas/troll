#include "animation/animator.h"

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/transform.hpp>

#include "core/scene-manager.h"
#include "core/troll-core.h"

namespace troll {

void Animator::Start() {
  if (animation_.has_translation()) {
    performers_.push_back(
        std::make_unique<TranslationPerformer>(animation_.translation()));
  }
  if (animation_.has_rotation()) {
    performers_.push_back(
        std::make_unique<RotationPerformer>(animation_.rotation()));
  }
  if (animation_.has_scaling()) {
    performers_.push_back(
        std::make_unique<ScalingPerformer>(animation_.scaling()));
  }
  if (animation_.has_frame_range()) {
    performers_.push_back(
        std::make_unique<FrameRangePerformer>(animation_.frame_range()));
  }
  if (animation_.has_frame_list()) {
    performers_.push_back(
        std::make_unique<FrameListPerformer>(animation_.frame_list()));
  }
  if (animation_.has_go_to()) {
    performers_.push_back(std::make_unique<GotoPerformer>(animation_.go_to()));
  }
  if (animation_.has_flash()) {
    performers_.push_back(std::make_unique<FlashPerformer>(animation_.flash()));
  }
}

bool Animator::Progress(int time_since_last_frame, SceneNode* scene_node) {
  const bool is_done = ranges::accumulate(
      performers_ |
          ranges::view::transform(
              [time_since_last_frame, scene_node](
                  const std::unique_ptr<Performer>& performer) -> bool {
                return performer->Progress(time_since_last_frame, scene_node);
              }),
      false, std::logical_or<bool>());

  return !(is_done && ++run_number_ == animation_.repeat());
}

void ScriptAnimator::Start() {
  if (MoveToNextAnimation()) {
    state_ = State::RUNNING;
    const auto* scene_node =
        Core::Instance().scene_manager().GetSceneNodeById(scene_node_id_);
    Core::Instance().scene_manager().Dirty(*scene_node);
  }
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
  if (!current_animator_->Progress(time_since_last_frame, scene_node) &&
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
