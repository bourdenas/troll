#include "animation/performer.h"

#include <glog/logging.h>

#include "animation/animator-manager.h"
#include "core/event-dispatcher.h"
#include "core/events.h"
#include "core/geometry.h"

namespace troll {

bool TranslationPerformer::Execute(SceneNode* scene_node) {
  Vector& pos = *scene_node->mutable_position();
  pos = pos + animation_.vec();
  return true;
}

bool RotationPerformer::Execute(SceneNode* scene_node) {
  // TODO(bourdenas): Implement SceneNode rotation.
  return true;
}

bool ScalingPerformer::Execute(SceneNode* scene_node) {
  // TODO(bourdenas): Implement SceneNode scaling.
  return true;
}

void FrameRangePerformer::Init(SceneNode* scene_node) {
  current_frame_ = animation_.start_frame();
  step_ = animation_.start_frame() < animation_.end_frame() ? 1 : -1;
  scene_node->set_frame_index(current_frame_);
  current_frame_ += step_;
}

bool FrameRangePerformer::Execute(SceneNode* scene_node) {
  // TODO: handle sprite alignment.
  if (current_frame_ == animation_.end_frame()) {
    current_frame_ = animation_.start_frame();
    scene_node->set_frame_index(current_frame_);
    current_frame_ += step_;
    return current_frame_ == animation_.end_frame();
  }

  scene_node->set_frame_index(current_frame_);
  current_frame_ += step_;
  return current_frame_ == animation_.end_frame();
}

void FrameListPerformer::Init(SceneNode* scene_node) {
  scene_node->set_frame_index(animation_.frame(current_frame_index_++));
}

bool FrameListPerformer::Execute(SceneNode* scene_node) {
  // TODO: handle sprite alignment.
  if (current_frame_index_ == animation_.frame_size()) {
    current_frame_index_ = 0;
    scene_node->set_frame_index(animation_.frame(current_frame_index_++));
    return current_frame_index_ == animation_.frame_size();
  }

  scene_node->set_frame_index(animation_.frame(current_frame_index_++));
  return current_frame_index_ == animation_.frame_size();
}

bool FlashPerformer::Execute(SceneNode* scene_node) {
  visible_ = !visible_;
  scene_node->set_visible(visible_);
  return true;
}

void GotoPerformer::Init(SceneNode* scene_node) {
  direction_ = animation_.destination() - scene_node->position();
  distance_ = geo::VectorLength(direction_);
  geo::VectorNormalise(&direction_);
}

bool GotoPerformer::Execute(SceneNode* scene_node) {
  if (distance_ > animation_.step()) {
    auto& pos = *scene_node->mutable_position();
    pos = pos + direction_ * animation_.step();
    distance_ -= animation_.step();
    return false;
  }

  *scene_node->mutable_position() = animation_.destination();
  return true;
}

bool TimerPerformer::Execute(SceneNode* _unused) { return true; }

void RunScriptPerformer::Init(SceneNode* scene_node) {
  AnimatorManager::Instance().Play(animation_.script_id(), scene_node->id());
  EventDispatcher::Instance().Register(
      Events::OnAnimationScriptTermination(scene_node->id(),
                                           animation_.script_id()),
      [this]() { finished_ = true; });
}

bool RunScriptPerformer::Execute(SceneNode* scene_node) { return finished_; }

}  // namespace troll
