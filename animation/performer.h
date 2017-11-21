#ifndef TROLL_ANIMATION_PERFORMER_H_
#define TROLL_ANIMATION_PERFORMER_H_

#include <vector>

#include "proto/animation.pb.h"
#include "proto/scene.pb.h"

namespace troll {

// Performers makes progress on a specific tasks of an animation, e.g.
// translation, rotation, frame change, etc.
class Performer {
 public:
  Performer() = default;
  virtual ~Performer() = default;

  // Returns true if the Animation was executed to end.
  virtual bool Progress(int time_since_last_frame, SceneNode* scene_node) = 0;

 protected:
  // Actual exectution of the animation aspect. Returns true, if animation is
  // complete.
  virtual bool Execute(SceneNode* scene_node) = 0;
};

template <class AnimationType>
class PerformerBase : public Performer {
 public:
  PerformerBase(const AnimationType& animation) : animation_(animation) {}
  ~PerformerBase() override = default;

  bool Progress(int time_since_last_frame, SceneNode* scene_node) override {
    wait_time_ += time_since_last_frame;
    while (animation_.delay() < wait_time_) {
      wait_time_ -= animation_.delay();

      // TODO: register SceneNode with collision checker.
      if (Execute(scene_node)) {
        return true;
      }
    }
    return false;
  }

 protected:
  const AnimationType& animation_;

 private:
  int wait_time_ = 0;
};

class TranslationPerformer : public PerformerBase<VectorAnimation> {
 public:
  TranslationPerformer(const VectorAnimation& animation)
      : PerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class RotationPerformer : public PerformerBase<VectorAnimation> {
 public:
  RotationPerformer(const VectorAnimation& animation)
      : PerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class ScalingPerformer : public PerformerBase<VectorAnimation> {
 public:
  ScalingPerformer(const VectorAnimation& animation)
      : PerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class FrameRangePerformer : public PerformerBase<FrameRangeAnimation> {
 public:
  FrameRangePerformer(const FrameRangeAnimation& animation)
      : PerformerBase<FrameRangeAnimation>(animation),
        current_frame_(animation.start_frame()) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  int current_frame_;
};

class FrameListPerformer : public PerformerBase<FrameListAnimation> {
 public:
  FrameListPerformer(const FrameListAnimation& animation)
      : PerformerBase<FrameListAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  int current_frame_index_ = 0;
};

class GotoPerformer : public PerformerBase<GotoAnimation> {
 public:
  GotoPerformer(const GotoAnimation& animation)
      : PerformerBase<GotoAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  // Direction vector from current position to destination.
  Vector direction_;

  // Distance to destination point.
  float distance_;
};

class FlashPerformer : public PerformerBase<FlashAnimation> {
 public:
  FlashPerformer(const FlashAnimation& animation)
      : PerformerBase<FlashAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  bool visible_ = true;
};

class TimerPerformer : public PerformerBase<TimerAnimation> {
 public:
  TimerPerformer(const TimerAnimation& animation)
      : PerformerBase<TimerAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* _unused) override;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_PERFORMER_H_
