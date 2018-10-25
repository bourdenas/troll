#include "action/action-manager.h"

#include <glog/logging.h>

namespace troll {

void ActionManager::Init() {
  executors_.emplace(Action::kNoop, std::make_unique<NoopExecutor>());
  executors_.emplace(Action::kQuit, std::make_unique<QuitExecutor>());
  executors_.emplace(Action::kEmit, std::make_unique<EmitExecutor>());

  executors_.emplace(Action::kChangeScene,
                     std::make_unique<ChangeSceneExecutor>());

  executors_.emplace(Action::kCreateSceneNode,
                     std::make_unique<CreateSceneNodeExecutor>());
  executors_.emplace(Action::kDestroySceneNode,
                     std::make_unique<DestroySceneNodeExecutor>());

  executors_.emplace(Action::kOnCollision,
                     std::make_unique<OnCollisionExecutor>());
  executors_.emplace(Action::kOnDetaching,
                     std::make_unique<OnDetachingExecutor>());

  executors_.emplace(Action::kPlayAnimationScript,
                     std::make_unique<PlayAnimationScriptExecutor>());
  executors_.emplace(Action::kStopAnimationScript,
                     std::make_unique<StopAnimationScriptExecutor>());
  executors_.emplace(Action::kPauseAnimationScript,
                     std::make_unique<PauseAnimationScriptExecutor>());

  executors_.emplace(Action::kDisplayText,
                     std::make_unique<DisplayTextExecutor>());

  executors_.emplace(Action::kImportModule,
                     std::make_unique<ImportModuleExecutor>());
  executors_.emplace(Action::kCall, std::make_unique<ScriptExecutor>());
}

void ActionManager::Execute(const Action& action) {
  const auto type = action.Action_case();
  LOG_IF(ERROR, executors_[type] == nullptr)
      << "Action: " << action.DebugString()
      << " is not registered with ActionManager and has no valid Executor.";
  executors_[type]->Execute(action);
}

Action ActionManager::Reverse(const Action& action) {
  const auto type = action.Action_case();
  LOG_IF(ERROR, executors_[type] == nullptr)
      << "Action: " << action.DebugString()
      << " is not registered with ActionManager and has no valid Executor.";
  return executors_[type]->Reverse(action);
}

}  // namespace troll
