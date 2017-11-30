#ifndef TROLL_ANIMATION_ANIMATOR_H_
#define TROLL_ANIMATION_ANIMATOR_H_

#include <memory>
#include <vector>

#include "animation/performer.h"
#include "proto/animation.pb.h"
#include "proto/scene.pb.h"

namespace troll {

// Runs an animation, which might contain different types of transformations on
// a scene node. See also, "proto/animation.proto".
class Animator {
 public:
  Animator(const Animation& animation) : animation_(animation) {}

  // Initialises the animator state.
  void Start();

  // Returns false, if the Animator finished.
  bool Progress(int time_since_last_frame, SceneNode* scene_node);

 private:
  const Animation& animation_;

  std::vector<std::unique_ptr<Performer>> performers_;
  int run_number_ = 0;
};

// Runs an animation script, i.e. a sequence of animations on a single
// scene-node.
class ScriptAnimator {
 public:
  ScriptAnimator(const AnimationScript& script,
                 const std::string& scene_node_id)
      : script_(script), scene_node_id_(scene_node_id) {}

  void Start();
  void Stop();
  void Pause();
  void Resume();

  // Returns false, if the script finished.
  bool Progress(int time_since_last_frame);

  bool is_running() const { return state_ == State::RUNNING; }
  bool is_finished() const { return state_ == State::FINISHED; }
  bool is_paused() const { return state_ == State::PAUSED; }

  const std::string& script_id() const { return script_.id(); }
  const std::string& scene_node_id() const { return scene_node_id_; }

 private:
  // Returns true, if there was a next animation.
  bool MoveToNextAnimation();

  enum class State {
    INIT,
    RUNNING,
    PAUSED,
    FINISHED,
  };

  const AnimationScript& script_;
  std::string scene_node_id_;

  State state_ = State::INIT;
  std::unique_ptr<Animator> current_animator_;
  int next_animation_index_ = 0;
};

}  // namespace troll

#endif  // TROLL_ANIMATION_ANIMATOR_H_
