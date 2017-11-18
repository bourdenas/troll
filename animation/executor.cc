#include "animation/executor.h"

namespace troll {

bool TranslationExecutor::Execute(SceneNode* scene_node) {
  scene_node->Translate(animation_.vec());
  return true;
}

bool RotationExecutor::Execute(SceneNode* scene_node) {
  scene_node->Rotate(animation_.vec());
  return true;
}

bool ScalingExecutor::Execute(SceneNode* scene_node) {
  scene_node->Scale(animation_.vec());
  return true;
}

bool FrameRangeExecutor::Execute(SceneNode* scene_node) {
  scene_node->set_frame(++current_frame_);
  // TODO: handle sprite alignment.
  return current_frame_ == animation_.end_frame();
}

bool FrameListExecutor::Execute(SceneNode* scene_node) {
  scene_node->set_frame(animation_.frame(current_frame_index_));
  // TODO: handle sprite alignment.
  return ++current_frame_index_ == animation_.frame_size();
}

bool GotoExecutor::Execute(SceneNode* scene_node) {
  if (distance_ > 0 && distance_ > animation_.step()) {
    // TODO: Figure out primitives that overload basic operators.
    // scene_node->Translate(direction_ * animation_.step());
    distance_ -= animation_.step();
  } else {
    scene_node->Position(animation_.destination());
    return true;
  }
  return false;
}

bool FlashExecutor::Execute(SceneNode* scene_node) {
  visible_ = !visible_;
  scene_node->set_visible(visible_);
  return true;
}

bool TimerExecutor::Execute(SceneNode* _unused) { return true; }

}  // namespace troll
