#include "core/action-manager.h"

namespace troll {

void ActionManager::Init() {
  executors_.emplace(Action::kQuit, std::make_unique<QuitExecutor>());
  executors_.emplace(Action::kEmit, std::make_unique<EmitExecutor>());

  executors_.emplace(Action::kChangeScene,
                     std::make_unique<ChangeSceneExecutor>());

  executors_.emplace(Action::kCreateSceneNode,
                     std::make_unique<CreateSceneNodeExecutor>());
  executors_.emplace(Action::kDestroySceneNode,
                     std::make_unique<DestroySceneNodeExecutor>());

  executors_.emplace(Action::kPlayAnimationScript,
                     std::make_unique<PlayAnimationScriptExecutor>());
  executors_.emplace(Action::kStopAnimationScript,
                     std::make_unique<StopAnimationScriptExecutor>());
  executors_.emplace(Action::kPauseAnimationScript,
                     std::make_unique<PauseAnimationScriptExecutor>());

  executors_.emplace(Action::kDisplayText,
                     std::make_unique<DisplayTextExecutor>());
}

void ActionManager::Execute(const Action& action) {
  const auto type = action.Action_case();
  executors_[type]->Execute(action);
}

}  // namespace troll
