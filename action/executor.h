#ifndef TROLL_ACTION_EXECUTOR_H_
#define TROLL_ACTION_EXECUTOR_H_

#include "proto/action.pb.h"

namespace troll {

class Executor {
 public:
  Executor() = default;
  virtual ~Executor() = default;

  virtual void Execute(const Action& action) const = 0;

  // Derived classes should override this function to returns an Action that
  // when executed will reverse the effect of the input action. If an action in
  // irreversible or if derived class does not override it, then a NoopAction is
  // returned.
  virtual Action Reverse(const Action& action) const;
};

class NoopExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class QuitExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class EmitExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class ChangeSceneExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class CreateSceneNodeExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class DestroySceneNodeExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class PositionSceneNodeExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class MoveSceneNodeExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class OnCollisionExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class OnDetachingExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class PlayAnimationScriptExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class StopAnimationScriptExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class PauseAnimationScriptExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class PlayAudioExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class StopAudioExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class PauseAudioExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class ResumeAudioExecutor : public Executor {
  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;
};

class DisplayTextExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class ScriptExecutor : public Executor {
  void Execute(const Action& action) const override;
};

}  // namespace troll

#endif  // TROLL_ACTION_EXECUTOR_H_
