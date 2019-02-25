#ifndef TROLL_ACTION_EXECUTOR_H_
#define TROLL_ACTION_EXECUTOR_H_

#include "core/core.h"
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
 public:
  QuitExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class EmitExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class ChangeSceneExecutor : public Executor {
 public:
  ChangeSceneExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class CreateSceneNodeExecutor : public Executor {
 public:
  CreateSceneNodeExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class DestroySceneNodeExecutor : public Executor {
 public:
  DestroySceneNodeExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class PositionSceneNodeExecutor : public Executor {
 public:
  PositionSceneNodeExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class MoveSceneNodeExecutor : public Executor {
 public:
  MoveSceneNodeExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class OnCollisionExecutor : public Executor {
 public:
  OnCollisionExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class OnDetachingExecutor : public Executor {
 public:
  OnDetachingExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class PlayAnimationScriptExecutor : public Executor {
 public:
  PlayAnimationScriptExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class StopAnimationScriptExecutor : public Executor {
 public:
  StopAnimationScriptExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class PauseAnimationScriptExecutor : public Executor {
 public:
  PauseAnimationScriptExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

class PlayAudioExecutor : public Executor {
 public:
  PlayAudioExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class StopAudioExecutor : public Executor {
 public:
  StopAudioExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class PauseAudioExecutor : public Executor {
 public:
  PauseAudioExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class ResumeAudioExecutor : public Executor {
 public:
  ResumeAudioExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;
  Action Reverse(const Action& action) const override;

 private:
  Core* core_;
};

class DisplayTextExecutor : public Executor {
  void Execute(const Action& action) const override;
};

class ScriptExecutor : public Executor {
 public:
  ScriptExecutor(Core* core) : core_(core) {}

  void Execute(const Action& action) const override;

 private:
  Core* core_;
};

}  // namespace troll

#endif  // TROLL_ACTION_EXECUTOR_H_
