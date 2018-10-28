#include "action/executor.h"

#include <absl/strings/str_cat.h>
#include <glog/logging.h>

#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/execution-context.h"
#include "core/scene-node-query.h"
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
  if (action.create_scene_node().scene_node().id().empty()) {
    static int i = 0;
    auto node = action.create_scene_node().scene_node();
    node.set_id(absl::StrCat("node_id#", i++));
    Core::Instance().scene_manager().AddSceneNode(node);
  } else {
    Core::Instance().scene_manager().AddSceneNode(
        action.create_scene_node().scene_node());
  }
}

namespace {
// Returns all scene node ids described by a node expression in an action.
std::vector<std::string> ResolveSceneNodes(const std::string& node_expression) {
  std::vector<std::string> node_ids;
  if (!node_expression.empty() && node_expression[0] == '$') {
    SceneNodeQuery query;
    if (!query.Parse(node_expression)) return node_ids;

    if (query.mode == SceneNodeQuery::RetrievalMode::GLOBAL) {
      node_ids = Core::Instance().scene_manager().GetSceneNodesByPattern(
          query.pattern);
    } else if (query.mode == SceneNodeQuery::RetrievalMode::LOCAL) {
      auto&& context_nodes = ExecutionContext::Instance().scene_nodes();
      node_ids = Core::Instance().scene_manager().GetSceneNodesByPattern(
          query.pattern, context_nodes);
    }
  } else {
    node_ids.push_back(node_expression);
  }

  return node_ids;
}
}  // namespace

void DestroySceneNodeExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.destroy_scene_node().scene_node().id());

  for (const auto& id : node_ids) {
    if (Core::Instance().scene_manager().GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "DestroySceneNodeExecutor: Cannot destroy SceneNode with id='"
          << id << "' that does not exist.";
      return;
    }
    Core::Instance().scene_manager().RemoveSceneNode(id);
  }
}

void OnCollisionExecutor::Execute(const Action& action) const {
  CollisionChecker::Instance().RegisterCollision(action.on_collision());
}

void OnDetachingExecutor::Execute(const Action& action) const {
  CollisionChecker::Instance().RegisterDetachment(action.on_detaching());
}

void PlayAnimationScriptExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.play_animation_script().scene_node().id());

  for (const auto& id : node_ids) {
    if (Core::Instance().scene_manager().GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "PlayAnimationScriptExecutor: Cannot apply animation script '"
          << action.play_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    AnimatorManager::Instance().Play(action.play_animation_script().script_id(),
                                     id);
  }
}

Action PlayAnimationScriptExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_stop_animation_script() = action.play_animation_script();
  return reverse;
}

void StopAnimationScriptExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.stop_animation_script().scene_node().id());

  for (const auto& id : node_ids) {
    if (Core::Instance().scene_manager().GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "StopAnimationScriptExecutor: Cannot stop animation script '"
          << action.stop_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    AnimatorManager::Instance().Stop(action.stop_animation_script().script_id(),
                                     id);
  }
}

Action StopAnimationScriptExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_play_animation_script() = action.stop_animation_script();
  return reverse;
}

void PauseAnimationScriptExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.pause_animation_script().scene_node().id());

  for (const auto& id : node_ids) {
    if (Core::Instance().scene_manager().GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "PauseAnimationScriptExecutor: Cannot pause animation script '"
          << action.pause_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    AnimatorManager::Instance().Pause(
        action.pause_animation_script().script_id(), id);
  }
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
