#include "core/executor.h"

#include "animation/animator-manager.h"
#include "core/troll-core.h"

namespace troll {

void QuitExecutor::Execute(const Action& action) const {
  Core::Instance().Halt();
}

void EmitExecutor::Execute(const Action& action) const {
  // TODO(bourdenas): Implement event handling system.
}

void ChangeSceneExecutor::Execute(const Action& action) const {
  Core::Instance().LoadScene(action.change_scene().scene_id());
}

void CreateSceneNodeExecutor::Execute(const Action& action) const {
  Core::Instance().scene_manager().AddSceneNode(
      action.create_scene_node().scene_node());
}

void DestroySceneNodeExecutor::Execute(const Action& action) const {
  Core::Instance().scene_manager().RemoveSceneNode(
      action.destroy_scene_node().scene_node().id());
}

void PlayAnimationScriptExecutor::Execute(const Action& action) const {
  AnimatorManager::Instance().Play(
      action.play_animation_script().script_id(),
      action.play_animation_script().scene_node_id());
}

void StopAnimationScriptExecutor::Execute(const Action& action) const {
  AnimatorManager::Instance().Stop(
      action.stop_animation_script().script_id(),
      action.stop_animation_script().scene_node_id());
}

void PauseAnimationScriptExecutor::Execute(const Action& action) const {
  AnimatorManager::Instance().Pause(
      action.pause_animation_script().script_id(),
      action.pause_animation_script().scene_node_id());
}

void DisplayTextExecutor::Execute(const Action& action) const {
  // TODO(bourdenas): Implement show text.
}

}  // namespace troll
