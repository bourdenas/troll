#include "animation/animator.h"

#include <range/v3/algorithm/any_of.hpp>

namespace troll {

void Animator::Start(SceneNode* scene_node) {
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
  if (animation_.has_flash()) {
    performers_.push_back(std::make_unique<FlashPerformer>(animation_.flash()));
  }
  if (animation_.has_go_to()) {
    performers_.push_back(std::make_unique<GotoPerformer>(animation_.go_to()));
  }
  if (animation_.has_timer()) {
    performers_.push_back(std::make_unique<TimerPerformer>(animation_.timer()));
  }
  if (animation_.has_run_script()) {
    performers_.push_back(
        std::make_unique<RunScriptPerformer>(animation_.run_script()));
  }

  for (auto& performer : performers_) {
    performer->Init(scene_node);
  }
}

bool Animator::Progress(int time_since_last_frame, SceneNode* scene_node) {
  return ranges::any_of(
      performers_, [time_since_last_frame,
                    scene_node](const std::unique_ptr<Performer>& performer) {
        return performer->Progress(time_since_last_frame, scene_node);
      });
}

}  // namespace troll
