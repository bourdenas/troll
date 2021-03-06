#include "animation/performer.h"

#include "animation/animator-manager.h"
#include "core/event-dispatcher.h"
#include "core/events.h"
#include "core/geometry.h"
#include "core/resource-manager.h"
#include "core/scene-manager.h"
#include "proto/event.pb.h"
#include "sound/audio-mixer.h"

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

namespace {
// Handles key frame changes on nodes taking care of sprite film alignments.
void SetSceneNodeFrame(int frame_index, VerticalAlign v_align,
                       HorizontalAlign h_align, SceneNode* node, Core* core) {
  const auto prev_aabb = core->scene_manager()->GetSceneNodeBoundingBox(*node);
  node->set_frame_index(frame_index);
  const auto next_aabb = core->scene_manager()->GetSceneNodeBoundingBox(*node);

  if (h_align == HorizontalAlign::RIGHT) {
    node->mutable_position()->set_x(node->position().x() + prev_aabb.width() -
                                    next_aabb.width());
  } else if (h_align == HorizontalAlign::HCENTRE) {
    node->mutable_position()->set_x(
        node->position().x() + (prev_aabb.width() - next_aabb.width()) / 2);
  }

  if (v_align == VerticalAlign::BOTTOM) {
    node->mutable_position()->set_y(node->position().y() + prev_aabb.height() -
                                    next_aabb.height());
  } else if (v_align == VerticalAlign::VCENTRE) {
    node->mutable_position()->set_y(
        node->position().y() + (prev_aabb.height() - next_aabb.height()) / 2);
  }
}
}  // namespace

void FrameRangePerformer::Start(SceneNode* scene_node) {
  current_frame_ = animation_.start_frame();
  step_ = animation_.start_frame() < animation_.end_frame() ? 1 : -1;

  SetSceneNodeFrame(current_frame_, animation_.vertical_align(),
                    animation_.horizontal_align(), scene_node, core_);
  current_frame_ += step_;
}

bool FrameRangePerformer::Execute(SceneNode* scene_node) {
  if (current_frame_ == animation_.end_frame()) {
    current_frame_ = animation_.start_frame();
    SetSceneNodeFrame(current_frame_, animation_.vertical_align(),
                      animation_.horizontal_align(), scene_node, core_);
    current_frame_ += step_;
    return current_frame_ == animation_.end_frame();
  }

  SetSceneNodeFrame(current_frame_, animation_.vertical_align(),
                    animation_.horizontal_align(), scene_node, core_);
  current_frame_ += step_;
  return current_frame_ == animation_.end_frame();
}

void FrameListPerformer::Start(SceneNode* scene_node) {
  SetSceneNodeFrame(animation_.frame(current_frame_index_++),
                    animation_.vertical_align(), animation_.horizontal_align(),
                    scene_node, core_);
}

bool FrameListPerformer::Execute(SceneNode* scene_node) {
  if (current_frame_index_ == animation_.frame_size()) {
    current_frame_index_ = 0;
    SetSceneNodeFrame(animation_.frame(current_frame_index_++),
                      animation_.vertical_align(),
                      animation_.horizontal_align(), scene_node, core_);
    return current_frame_index_ == animation_.frame_size();
  }

  SetSceneNodeFrame(animation_.frame(current_frame_index_++),
                    animation_.vertical_align(), animation_.horizontal_align(),
                    scene_node, core_);
  return current_frame_index_ == animation_.frame_size();
}

bool FlashPerformer::Execute(SceneNode* scene_node) {
  visible_ = !visible_;
  scene_node->set_visible(visible_);
  return true;
}

bool GotoPerformer::Execute(SceneNode* scene_node) {
  Vector direction = animation_.destination() - scene_node->position();
  double distance = geo::VectorLength(direction);
  geo::VectorNormalise(&direction);

  if (distance > animation_.step()) {
    auto& pos = *scene_node->mutable_position();
    pos = pos + direction * animation_.step();
    distance -= animation_.step();
    return false;
  }

  *scene_node->mutable_position() = animation_.destination();
  return true;
}

bool TimerPerformer::Execute(SceneNode* _unused) { return true; }

void RunScriptPerformer::Start(SceneNode* scene_node) {
  core_->animator_manager()->Play(
      core_->resource_manager()->GetAnimationScript(animation_.script_id()),
      scene_node->id());
  core_->event_dispatcher()->Register(
      Events::OnAnimationScriptTermination(scene_node->id(),
                                           animation_.script_id())
          .event_id(),
      [this](const Event&) { finished_ = true; });
}

void RunScriptPerformer::Stop(const SceneNode& scene_node) {
  core_->animator_manager()->Stop(animation_.script_id(), scene_node.id());
}

void RunScriptPerformer::Pause(const SceneNode& scene_node) {
  core_->animator_manager()->Pause(animation_.script_id(), scene_node.id());
}

void RunScriptPerformer::Resume(const SceneNode& scene_node) {
  core_->animator_manager()->Resume(animation_.script_id(), scene_node.id());
}

bool RunScriptPerformer::Execute(SceneNode* scene_node) { return finished_; }

void SfxPerformer::Start(SceneNode* scene_node) {
  auto&& on_done = [this]() { finished_ = false; };
  if (!animation_.audio().track().empty()) {
    core_->audio_mixer()->PlayMusic(animation_.audio().track(0).id(),
                                    animation_.repeat(), on_done);
  } else if (!animation_.audio().sfx().empty()) {
    core_->audio_mixer()->PlaySound(animation_.audio().sfx(0).id(),
                                    animation_.repeat(), on_done);
  }
}

void SfxPerformer::Stop(const SceneNode& scene_node) {
  if (!animation_.audio().track().empty()) {
    core_->audio_mixer()->StopMusic();
  } else if (!animation_.audio().sfx().empty()) {
    core_->audio_mixer()->StopSound(animation_.audio().sfx(0).id());
  }
}

bool SfxPerformer::Execute(SceneNode* _unused) { return finished_; }

}  // namespace troll
