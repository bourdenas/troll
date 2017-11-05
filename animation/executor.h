#ifndef TROLL_ANIMATION_EXECUTOR_H_
#define TROLL_ANIMATION_EXECUTOR_H_

#include <vector>

#include "core/scene.h"
#include "proto/animation.pb.h"

namespace troll {

// Executors perform a specific tasks of an animation, e.g. translation,
// rotation, frame change, etc.
class Executor {
 public:
  Executor() = default;
  virtual ~Executor() = default;

  // Returns true if the Animation was executed to end.
  virtual bool Progress(int time_since_last_frame, SceneNode* scene_node) = 0;

 protected:
  // Actual exectution of the animation aspect. Returns true, if animation is
  // complete.
  virtual bool Execute(SceneNode* scene_node) = 0;
};

template <class AnimationType>
class ExecutorBase : public Executor {
 public:
  ExecutorBase(const AnimationType& animation) : animation_(animation) {}
  ~ExecutorBase() override = default;

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

class TranslationExecutor : public ExecutorBase<TranslationAnimation> {
 public:
  TranslationExecutor(const TranslationAnimation& animation)
      : ExecutorBase<TranslationAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class RotationExecutor : public ExecutorBase<RotationAnimation> {
 public:
  RotationExecutor(const RotationAnimation& animation)
      : ExecutorBase<RotationAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class ScalingExecutor : public ExecutorBase<ScalingAnimation> {
 public:
  ScalingExecutor(const ScalingAnimation& animation)
      : ExecutorBase<ScalingAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;
};

class FrameRangeExecutor : public ExecutorBase<FrameRangeAnimation> {
 public:
  FrameRangeExecutor(const FrameRangeAnimation& animation)
      : ExecutorBase<FrameRangeAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  int current_frame_;
};

class FrameListExecutor : public ExecutorBase<FrameListAnimation> {
 public:
  FrameListExecutor(const FrameListAnimation& animation)
      : ExecutorBase<FrameListAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  int current_frame_index_ = 0;
};

class GotoExecutor : public ExecutorBase<GotoAnimation> {
 public:
  GotoExecutor(const GotoAnimation& animation)
      : ExecutorBase<GotoAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  // Direction vector from current position to destination.
  Vector direction_;

  // Distance to destination point.
  float distance_;
};

class FlashExecutor : public ExecutorBase<FlashAnimation> {
 public:
  FlashExecutor(const FlashAnimation& animation)
      : ExecutorBase<FlashAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  bool visible_ = true;
};

class TimerExecutor : public ExecutorBase<TimerAnimation> {
 public:
  TimerExecutor(const TimerAnimation& animation)
      : ExecutorBase<TimerAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* _unused) override;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_EXECUTOR_H_
