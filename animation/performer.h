#ifndef TROLL_ANIMATION_PERFORMER_H_
#define TROLL_ANIMATION_PERFORMER_H_

#include <vector>

#include "core/core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {

// Performers makes progress on a specific tasks of an animation, e.g.
// translation, rotation, frame change, etc.
class Performer {
 public:
  Performer() = default;
  virtual ~Performer() = default;

  // Override if performer needs initialisation.
  virtual void Start(SceneNode* scene_node) {}

  // Override if performer needs to clean-up.
  virtual void Stop(const SceneNode& scene_node) {}

  // Override if performer needs to custom logic for pause.
  virtual void Pause(const SceneNode& scene_node) {}

  // Override if performer needs to custom logic for resume.
  virtual void Resume(const SceneNode& scene_node) {}

  // Returns true if the Animation was executed to end.
  virtual bool Progress(int time_since_last_frame, SceneNode* scene_node) = 0;

 protected:
  // Actual exectution of the animation aspect. Returns true if animation is
  // finished.
  virtual bool Execute(SceneNode* scene_node) = 0;
};

// Base class for animation performers that are repeatable and have a delay.
template <class AnimationType>
class RepeatablePerformerBase : public Performer {
 public:
  RepeatablePerformerBase(const AnimationType& animation)
      : animation_(animation) {}
  ~RepeatablePerformerBase() override = default;

  bool Progress(int time_since_last_frame, SceneNode* scene_node) override {
    if (animation_.delay() == 0) {
      return RepeatableExecute(scene_node);
    }

    wait_time_ += time_since_last_frame;
    while (animation_.delay() <= wait_time_) {
      wait_time_ -= animation_.delay();

      if (RepeatableExecute(scene_node)) {
        return true;
      }
    }
    return false;
  }

 protected:
  // Returns true if animation is finished cannot be repeated anymore.
  bool RepeatableExecute(SceneNode* scene_node) {
    return Execute(scene_node) && ++run_number_ == animation_.repeat();
  }

  const AnimationType& animation_;

 private:
  int wait_time_ = 0;
  int run_number_ = 0;
};

// Base class for animation performers that are not repeatable.
template <class AnimationType>
class OneOffPerformerBase : public Performer {
 public:
  OneOffPerformerBase(const AnimationType& animation) : animation_(animation) {}
  ~OneOffPerformerBase() override = default;

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

// Base class for animation performers that are applied instantly, no delay.
template <class AnimationType>
class InstantPerformerBase : public Performer {
 public:
  InstantPerformerBase(const AnimationType& animation)
      : animation_(animation) {}
  ~InstantPerformerBase() override = default;

  bool Progress(int time_since_last_frame, SceneNode* scene_node) override {
    return Execute(scene_node);
  }

 protected:
  const AnimationType& animation_;
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
  FrameRangePerformer(const FrameRangeAnimation& animation, Core* core)
      : RepeatablePerformerBase<FrameRangeAnimation>(animation),
        core_(core),
        current_frame_(animation.start_frame()) {}

  void Start(SceneNode* scene_node) override;

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  Core* core_;

  int current_frame_ = 0;
  int step_ = 0;
};

class FrameListPerformer : public RepeatablePerformerBase<FrameListAnimation> {
 public:
  FrameListPerformer(const FrameListAnimation& animation, Core* core)
      : RepeatablePerformerBase<FrameListAnimation>(animation), core_(core) {}

  void Start(SceneNode* scene_node) override;

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  Core* core_;

  int current_frame_index_ = 0;
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

class GotoPerformer : public OneOffPerformerBase<GotoAnimation> {
 public:
  GotoPerformer(const GotoAnimation& animation)
      : OneOffPerformerBase<GotoAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  // Direction vector from current position to destination.
  Vector direction_;

  // Distance to destination point.
  double distance_;
};

class TimerPerformer : public OneOffPerformerBase<TimerAnimation> {
 public:
  TimerPerformer(const TimerAnimation& animation)
      : OneOffPerformerBase<TimerAnimation>(animation) {}

 protected:
  bool Execute(SceneNode* _unused) override;
};

class RunScriptPerformer : public InstantPerformerBase<RunScriptAnimation> {
 public:
  RunScriptPerformer(const RunScriptAnimation& animation, Core* core)
      : InstantPerformerBase<RunScriptAnimation>(animation), core_(core) {}

  void Start(SceneNode* scene_node) override;
  void Stop(const SceneNode& scene_node) override;
  void Pause(const SceneNode& scene_node) override;
  void Resume(const SceneNode& scene_node) override;

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  Core* core_;

  bool finished_ = false;
};

class SfxPerformer : public InstantPerformerBase<SfxAnimation> {
 public:
  SfxPerformer(const SfxAnimation& animation, Core* core)
      : InstantPerformerBase<SfxAnimation>(animation), core_(core) {}

  void Start(SceneNode* scene_node) override;
  void Stop(const SceneNode& scene_node) override;

 protected:
  bool Execute(SceneNode* scene_node) override;

 private:
  Core* core_;

  bool finished_ = false;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_PERFORMER_H_
