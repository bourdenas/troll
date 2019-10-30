#include "animation/animator.h"

#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/remove_if.hpp>

namespace troll {

void Animator::Start(const Animation& animation, SceneNode* scene_node,
                     Core* core) {
  wait_for_all_ = animation.termination() == Animation::ALL;

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
        std::make_unique<FrameRangePerformer>(animation.frame_range(), core));
  }
  if (animation.has_frame_list()) {
    performers_.push_back(
        std::make_unique<FrameListPerformer>(animation.frame_list(), core));
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
        std::make_unique<RunScriptPerformer>(animation.run_script(), core));
  }
  if (animation.has_sfx()) {
    performers_.push_back(
        std::make_unique<SfxPerformer>(animation.sfx(), core));
  }

  for (auto& performer : performers_) {
    performer->Start(scene_node);
  }
}

void Animator::Stop(const SceneNode& scene_node) {
  for (auto& performer : performers_) {
    performer->Stop(scene_node);
  }
}

void Animator::Pause(const SceneNode& scene_node) {
  for (auto& performer : performers_) {
    performer->Pause(scene_node);
  }
}

void Animator::Resume(const SceneNode& scene_node) {
  for (auto& performer : performers_) {
    performer->Resume(scene_node);
  }
}

bool Animator::Progress(int time_since_last_frame, SceneNode* scene_node) {
  const bool finished = wait_for_all_
                            ? ProgressAll(time_since_last_frame, scene_node)
                            : ProgressAny(time_since_last_frame, scene_node);
  if (finished) {
    Stop(*scene_node);
  }
  return finished;
}

bool Animator::ProgressAny(int time_since_last_frame, SceneNode* scene_node) {
  return ranges::any_of(
      performers_, [time_since_last_frame,
                    scene_node](const std::unique_ptr<Performer>& performer) {
        return performer->Progress(time_since_last_frame, scene_node);
      });
}

bool Animator::ProgressAll(int time_since_last_frame, SceneNode* scene_node) {
  const auto it = std::remove_if(
      performers_.begin(), performers_.end(),
      [time_since_last_frame,
       scene_node](const std::unique_ptr<Performer>& performer) {
        return performer->Progress(time_since_last_frame, scene_node);
      });
  performers_.erase(it, performers_.end());
  return performers_.empty();
}

}  // namespace troll
