#include "animation/animator.h"

#include <range/v3/algorithm/any_of.hpp>

namespace troll {

void Animator::Start(const Animation& animation, SceneNode* scene_node) {
  if (animation.has_translation()) {
    performers_.push_back(
        std::make_unique<TranslationPerformer>(animation.translation()));
  }
  if (animation.has_rotation()) {
    performers_.push_back(
        std::make_unique<RotationPerformer>(animation.rotation()));
  }
  if (animation.has_scaling()) {
    performers_.push_back(
        std::make_unique<ScalingPerformer>(animation.scaling()));
  }
  if (animation.has_frame_range()) {
    performers_.push_back(
        std::make_unique<FrameRangePerformer>(animation.frame_range()));
  }
  if (animation.has_frame_list()) {
    performers_.push_back(
        std::make_unique<FrameListPerformer>(animation.frame_list()));
  }
  if (animation.has_flash()) {
    performers_.push_back(std::make_unique<FlashPerformer>(animation.flash()));
  }
  if (animation.has_go_to()) {
    performers_.push_back(std::make_unique<GotoPerformer>(animation.go_to()));
  }
  if (animation.has_timer()) {
    performers_.push_back(std::make_unique<TimerPerformer>(animation.timer()));
  }
  if (animation.has_run_script()) {
    performers_.push_back(
        std::make_unique<RunScriptPerformer>(animation.run_script()));
  }

  for (auto& performer : performers_) {
    performer->Start(scene_node);
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
