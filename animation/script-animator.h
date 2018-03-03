#ifndef TROLL_ANIMATION_SCRIPT_ANIMATOR_H_
#define TROLL_ANIMATION_SCRIPT_ANIMATOR_H_

#include <memory>
#include <vector>

#include "animation/animator.h"
#include "proto/animation.pb.h"

namespace troll {

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

  // Returns true if the script finished.
  bool Progress(int time_since_last_frame);

  bool is_running() const { return state_ == State::RUNNING; }
  bool is_finished() const { return state_ == State::FINISHED; }
  bool is_paused() const { return state_ == State::PAUSED; }

  const std::string& script_id() const { return script_.id(); }
  const std::string& scene_node_id() const { return scene_node_id_; }

 private:
  // Returns true if there is a next animation, false if the script is finished.
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

#endif  // TROLL_ANIMATION_SCRIPT_ANIMATOR_H_
