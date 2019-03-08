#include "action/executor.h"

#include <absl/strings/str_cat.h>
#include <glog/logging.h>

#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/core.h"
#include "core/resource-manager.h"
#include "core/scene-node-query.h"
#include "sound/audio-mixer.h"

namespace troll {

Action Executor::Reverse(const Action& action) const {
  Action noop;
  noop.mutable_noop();
  return noop;
}

void NoopExecutor::Execute(const Action& action) const {}

void QuitExecutor::Execute(const Action& action) const { core_->Halt(); }

void EmitExecutor::Execute(const Action& action) const {
  // TODO(bourdenas): Implement event handling system.
}

void ChangeSceneExecutor::Execute(const Action& action) const {
  core_->LoadScene(action.change_scene().scene());
}

void CreateSceneNodeExecutor::Execute(const Action& action) const {
  if (action.create_scene_node().scene_node().id().empty()) {
    static int i = 0;
    auto node = action.create_scene_node().scene_node();
    node.set_id(absl::StrCat("node_id#", i++));
    core_->scene_manager()->AddSceneNode(node);
  } else {
    core_->scene_manager()->AddSceneNode(
        action.create_scene_node().scene_node());
  }
}

namespace {
// Returns all scene node ids described by a node expression in an action.
std::vector<std::string> ResolveSceneNodes(const std::string& node_expression,
                                           Core* core) {
  std::vector<std::string> node_ids;
  if (!node_expression.empty() && node_expression[0] == '$') {
    SceneNodeQuery query;
    if (!query.Parse(node_expression)) return node_ids;

    if (query.mode == SceneNodeQuery::RetrievalMode::GLOBAL) {
      node_ids = core->scene_manager()->GetSceneNodesByPattern(query.pattern);
    } else if (query.mode == SceneNodeQuery::RetrievalMode::LOCAL) {
      auto&& context_nodes = core->collision_checker()->collision_context();
      node_ids = core->scene_manager()->GetSceneNodesByPattern(query.pattern,
                                                               context_nodes);
    }
  } else {
    node_ids.push_back(node_expression);
  }

  return node_ids;
}
}  // namespace

void DestroySceneNodeExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.destroy_scene_node().scene_node().id(), core_);

  for (const auto& id : node_ids) {
    if (core_->scene_manager()->GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "DestroySceneNodeExecutor: Cannot destroy SceneNode with id='"
          << id << "' that does not exist.";
      return;
    }
    core_->scene_manager()->RemoveSceneNode(id);
  }
}

void PositionSceneNodeExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.position_scene_node().scene_node_id(), core_);

  for (const auto& id : node_ids) {
    auto* node = core_->scene_manager()->GetSceneNodeById(id);
    if (node == nullptr) {
      LOG(WARNING) << "PositionSceneNodeExecutor: Cannot set position for "
                      "SceneNode with id='"
                   << id << "', because it does not exist.";
      return;
    }

    core_->scene_manager()->Dirty(*node);
    *node->mutable_position() = action.position_scene_node().vec();
  }
}

void MoveSceneNodeExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.move_scene_node().scene_node_id(), core_);

  for (const auto& id : node_ids) {
    auto* node = core_->scene_manager()->GetSceneNodeById(id);
    if (node == nullptr) {
      LOG(WARNING) << "MoveSceneNodeExecutor: Cannot move SceneNode with id='"
                   << id << "' that does not exist.";
      return;
    }

    core_->scene_manager()->Dirty(*node);
    auto* pos = node->mutable_position();
    const auto& move = action.move_scene_node().vec();
    pos->set_x(pos->x() + move.x());
    pos->set_y(pos->y() + move.y());
    pos->set_z(pos->z() + move.z());
  }
}

Action MoveSceneNodeExecutor::Reverse(const Action& action) const {
  auto vec = action.move_scene_node().vec();
  if (vec.has_x()) vec.set_x(-vec.x());
  if (vec.has_y()) vec.set_y(-vec.y());
  if (vec.has_z()) vec.set_z(-vec.z());

  Action reverse;
  *reverse.mutable_move_scene_node()->mutable_vec() = vec;
  return reverse;
}

void OnCollisionExecutor::Execute(const Action& action) const {
  core_->collision_checker()->RegisterCollision(action.on_collision());
}

void OnDetachingExecutor::Execute(const Action& action) const {
  core_->collision_checker()->RegisterDetachment(action.on_detaching());
}

void PlayAnimationScriptExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.play_animation_script().scene_node_id(), core_);

  for (const auto& id : node_ids) {
    if (core_->scene_manager()->GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "PlayAnimationScriptExecutor: Cannot apply animation script '"
          << action.play_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    const auto& script = action.play_animation_script().has_script()
                             ? action.play_animation_script().script()
                             : core_->resource_manager()->GetAnimationScript(
                                   action.play_animation_script().script_id());
    core_->animator_manager()->Play(script, id);
  }
}

Action PlayAnimationScriptExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_stop_animation_script() = action.play_animation_script();
  return reverse;
}

void StopAnimationScriptExecutor::Execute(const Action& action) const {
  auto&& node_ids =
      ResolveSceneNodes(action.stop_animation_script().scene_node_id(), core_);

  for (const auto& id : node_ids) {
    if (core_->scene_manager()->GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "StopAnimationScriptExecutor: Cannot stop animation script '"
          << action.stop_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    core_->animator_manager()->Stop(action.stop_animation_script().script_id(),
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
      ResolveSceneNodes(action.pause_animation_script().scene_node_id(), core_);

  for (const auto& id : node_ids) {
    if (core_->scene_manager()->GetSceneNodeById(id) == nullptr) {
      LOG(WARNING)
          << "PauseAnimationScriptExecutor: Cannot pause animation script '"
          << action.pause_animation_script().script_id()
          << "' on SceneNode with id='" << id << "' that does not exist.";
      return;
    }

    core_->animator_manager()->Pause(
        action.pause_animation_script().script_id(), id);
  }
}

void PlayAudioExecutor::Execute(const Action& action) const {
  const auto& audio = action.play_audio();
  if (audio.has_track_id()) {
    core_->audio_mixer()->PlayMusic(audio.track_id(), audio.repeat(), {});
  } else {
    core_->audio_mixer()->PlaySound(audio.sfx_id(), audio.repeat(), {});
  }
}

Action PlayAudioExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_stop_audio() = action.play_audio();
  return reverse;
}

void StopAudioExecutor::Execute(const Action& action) const {
  const auto& audio = action.stop_audio();
  if (audio.has_track_id()) {
    core_->audio_mixer()->StopMusic();
  } else if (audio.has_sfx_id()) {
    core_->audio_mixer()->StopSound(audio.sfx_id());
  }
}

Action StopAudioExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_play_audio() = action.stop_audio();
  return reverse;
}

void PauseAudioExecutor::Execute(const Action& action) const {
  const auto& audio = action.pause_audio();
  if (audio.has_track_id()) {
    core_->audio_mixer()->PauseMusic();
  } else if (audio.has_sfx_id()) {
    core_->audio_mixer()->PauseSound(audio.sfx_id());
  }
}

Action PauseAudioExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_resume_audio() = action.pause_audio();
  return reverse;
}

void ResumeAudioExecutor::Execute(const Action& action) const {
  const auto& audio = action.resume_audio();
  if (audio.has_track_id()) {
    core_->audio_mixer()->ResumeMusic();
  } else if (audio.has_sfx_id()) {
    core_->audio_mixer()->ResumeSound(audio.sfx_id());
  }
}

Action ResumeAudioExecutor::Reverse(const Action& action) const {
  Action reverse;
  *reverse.mutable_pause_audio() = action.resume_audio();
  return reverse;
}

void DisplayTextExecutor::Execute(const Action& action) const {
  // TODO(bourdenas): Implement show text.
}

}  // namespace troll
