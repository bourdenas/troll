#include "action/executor.h"

#include <glog/logging.h>

#include "animation/animator-manager.h"
#include "core/troll-core.h"
#include "scripting/script-manager.h"

namespace troll {

Action Executor::Reverse(const Action& action) const {
  Action noop;
  noop.mutable_noop();
  return noop;
}

void NoopExecutor::Execute(const Action& action) const {}

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
  const auto& scene_node_id = action.destroy_scene_node().scene_node().id();
  if (Core::Instance().scene_manager().GetSceneNodeById(scene_node_id) ==
      nullptr) {
    DLOG(WARNING)
        << "DestroySceneNodeExecutor: Cannot destroy SceneNode with id='"
        << scene_node_id << "' that does not exist.";
    return;
  }

  Core::Instance().scene_manager().RemoveSceneNode(scene_node_id);
}

void PlayAnimationScriptExecutor::Execute(const Action& action) const {
  const auto& scene_node_id = action.play_animation_script().scene_node_id();
  if (Core::Instance().scene_manager().GetSceneNodeById(scene_node_id) ==
      nullptr) {
    DLOG(WARNING)
        << "PlayAnimationScriptExecutor: Cannot apply animation script '"
        << action.play_animation_script().script_id()
        << "' on SceneNode with id='" << scene_node_id
        << "' that does not exist.";
    return;
  }

  AnimatorManager::Instance().Play(action.play_animation_script().script_id(),
                                   scene_node_id);
}

Action PlayAnimationScriptExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_stop_animation_script() = action.play_animation_script();
  return reverse;
}

void StopAnimationScriptExecutor::Execute(const Action& action) const {
  const auto& scene_node_id = action.stop_animation_script().scene_node_id();
  if (Core::Instance().scene_manager().GetSceneNodeById(scene_node_id) ==
      nullptr) {
    DLOG(WARNING)
        << "StopAnimationScriptExecutor: Cannot stop animation script '"
        << action.stop_animation_script().script_id()
        << "' on SceneNode with id='" << scene_node_id
        << "' that does not exist.";
    return;
  }

  AnimatorManager::Instance().Stop(action.stop_animation_script().script_id(),
                                   scene_node_id);
}

Action StopAnimationScriptExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_play_animation_script() = action.stop_animation_script();
  return reverse;
}

void PauseAnimationScriptExecutor::Execute(const Action& action) const {
  const auto& scene_node_id = action.pause_animation_script().scene_node_id();
  if (Core::Instance().scene_manager().GetSceneNodeById(scene_node_id) ==
      nullptr) {
    DLOG(WARNING)
        << "PauseAnimationScriptExecutor: Cannot pause animation script '"
        << action.pause_animation_script().script_id()
        << "' on SceneNode with id='" << scene_node_id
        << "' that does not exist.";
    return;
  }

  AnimatorManager::Instance().Pause(action.pause_animation_script().script_id(),
                                    scene_node_id);
}

void DisplayTextExecutor::Execute(const Action& action) const {
  // TODO(bourdenas): Implement show text.
}

void ImportModuleExecutor::Execute(const Action& action) const {
  ScriptManager::Instance().ImportModule(action.import_module().module());
}

void ScriptExecutor::Execute(const Action& action) const {
  ScriptManager::Instance().Call(action.call().module(),
                                 action.call().function());
}

}  // namespace troll
