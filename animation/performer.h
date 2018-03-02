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
  // Actual exectution of the animation aspect. Returns true if animation is
  // finished.
  virtual bool Execute(SceneNode* scene_node) = 0;
};

template <class AnimationType>
class PerformerBase : public Performer {
 public:
  PerformerBase(const AnimationType& animation) : animation_(animation) {}
  ~PerformerBase() override = default;

  bool Progress(int time_since_last_frame, SceneNode* scene_node) override {
    wait_time_ += time_since_last_frame;
    while (animation_.delay() <= wait_time_) {
      wait_time_ -= animation_.delay();

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

template <class AnimationType>
class RepeatablePerformerBase : public Performer {
 public:
  RepeatablePerformerBase(const AnimationType& animation)
      : animation_(animation) {}
  ~RepeatablePerformerBase() override = default;

  bool Progress(int time_since_last_frame, SceneNode* scene_node) override {
    wait_time_ += time_since_last_frame;
    while (animation_.delay() <= wait_time_) {
      wait_time_ -= animation_.delay();

      if (Execute(scene_node) && ++run_number_ == animation_.repeat()) {
        return true;
      }
    }
    return false;
  }

 protected:
  const AnimationType& animation_;

 private:
  int wait_time_ = 0;
  int run_number_ = 0;
};

class TranslationPerformer : public RepeatablePerformerBase<VectorAnimation> {
 public:
  TranslationPerformer(const VectorAnimation& animation)
      : RepeatablePerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class RotationPerformer : public RepeatablePerformerBase<VectorAnimation> {
 public:
  RotationPerformer(const VectorAnimation& animation)
      : RepeatablePerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class ScalingPerformer : public RepeatablePerformerBase<VectorAnimation> {
 public:
  ScalingPerformer(const VectorAnimation& animation)
      : RepeatablePerformerBase<VectorAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class FrameRangePerformer
    : public RepeatablePerformerBase<FrameRangeAnimation> {
 public:
  FrameRangePerformer(const FrameRangeAnimation& animation)
      : RepeatablePerformerBase<FrameRangeAnimation>(animation),
        current_frame_(animation.start_frame()) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  int current_frame_;
};

class FrameListPerformer : public RepeatablePerformerBase<FrameListAnimation> {
 public:
  FrameListPerformer(const FrameListAnimation& animation)
      : RepeatablePerformerBase<FrameListAnimation>(animation) {}

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
  void Init(SceneNode* scene_node) override;
  bool Execute(SceneNode* scene_node) override;

 private:
  // Direction vector from current position to destination.
  Vector direction_;

  // Distance to destination point.
  double distance_;
};

class FlashPerformer : public RepeatablePerformerBase<FlashAnimation> {
 public:
  FlashPerformer(const FlashAnimation& animation)
      : RepeatablePerformerBase<FlashAnimation>(animation) {}

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
