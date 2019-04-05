#include "action/action-manager.h"

#include <glog/logging.h>

namespace troll {

ActionManager::ActionManager(Core* core) {
  executors_.emplace(Action::kNoop, std::make_unique<NoopExecutor>());
  executors_.emplace(Action::kQuit, std::make_unique<QuitExecutor>(core));
  executors_.emplace(Action::kEmit, std::make_unique<EmitExecutor>(core));

  executors_.emplace(Action::kChangeScene,
                     std::make_unique<ChangeSceneExecutor>(core));

  executors_.emplace(Action::kCreateSceneNode,
                     std::make_unique<CreateSceneNodeExecutor>(core));
  executors_.emplace(Action::kDestroySceneNode,
                     std::make_unique<DestroySceneNodeExecutor>(core));

  executors_.emplace(Action::kPositionSceneNode,
                     std::make_unique<PositionSceneNodeExecutor>(core));
  executors_.emplace(Action::kMoveSceneNode,
                     std::make_unique<MoveSceneNodeExecutor>(core));

  executors_.emplace(Action::kOnCollision,
                     std::make_unique<OnCollisionExecutor>(core));
  executors_.emplace(Action::kOnDetaching,
                     std::make_unique<OnDetachingExecutor>(core));

  executors_.emplace(Action::kPlayAnimationScript,
                     std::make_unique<PlayAnimationScriptExecutor>(core));
  executors_.emplace(Action::kStopAnimationScript,
                     std::make_unique<StopAnimationScriptExecutor>(core));
  executors_.emplace(Action::kPauseAnimationScript,
                     std::make_unique<PauseAnimationScriptExecutor>(core));

  executors_.emplace(Action::kPlayAudio,
                     std::make_unique<PlayAudioExecutor>(core));
  executors_.emplace(Action::kStopAudio,
                     std::make_unique<StopAudioExecutor>(core));
  executors_.emplace(Action::kPauseAudio,
                     std::make_unique<PauseAudioExecutor>(core));
  executors_.emplace(Action::kResumeAudio,
                     std::make_unique<ResumeAudioExecutor>(core));

  executors_.emplace(Action::kDisplayText,
                     std::make_unique<DisplayTextExecutor>());
}

void ActionManager::Execute(const Action& action) const {
  const auto type = action.Action_case();
  const auto it = executors_.find(type);
  if (it == executors_.end()) {
    LOG(ERROR)
        << "Action: " << action.DebugString()
        << " is not registered with ActionManager and has no valid Executor.";
    return;
  }
  it->second->Execute(action);
}

Action ActionManager::Reverse(const Action& action) const {
  const auto type = action.Action_case();
  const auto it = executors_.find(type);
  if (it == executors_.end()) {
    LOG(ERROR)
        << "Action: " << action.DebugString()
        << " is not registered with ActionManager and has no valid Executor.";
    return Action();
  }
  return it->second->Reverse(action);
}

}  // namespace troll
