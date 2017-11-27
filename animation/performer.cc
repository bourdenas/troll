#include "animation/performer.h"

namespace troll {

bool TranslationPerformer::Execute(SceneNode* scene_node) {
  auto* pos = scene_node->mutable_position();
  pos->set_x(pos->x() + animation_.vec().x());
  pos->set_y(pos->y() + animation_.vec().y());
  pos->set_z(pos->z() + animation_.vec().z());
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

bool FrameRangePerformer::Execute(SceneNode* scene_node) {
  scene_node->set_frame_index(++current_frame_);
  // TODO: handle sprite alignment.
  if (current_frame_ == animation_.end_frame()) {
    current_frame_ = animation_.start_frame() - 1;
    return true;
  }
  return current_frame_ == animation_.end_frame();
}

bool FrameListPerformer::Execute(SceneNode* scene_node) {
  scene_node->set_frame_index(animation_.frame(current_frame_index_));
  // TODO: handle sprite alignment.
  return ++current_frame_index_ == animation_.frame_size();
}

bool GotoPerformer::Execute(SceneNode* scene_node) {
  if (distance_ > 0 && distance_ > animation_.step()) {
    // TODO: Figure out primitives that overload basic operators.
    // scene_node->Translate(direction_ * animation_.step());
    distance_ -= animation_.step();
  } else {
    *scene_node->mutable_position() = animation_.destination();
    return true;
  }
  return false;
}

bool FlashPerformer::Execute(SceneNode* scene_node) {
  visible_ = !visible_;
  scene_node->set_visible(visible_);
  return true;
}

bool TimerPerformer::Execute(SceneNode* _unused) { return true; }

}  // namespace troll