#include "core/scene-manager.h"

#include <glog/logging.h>

#include "action/action-manager.h"
#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/resource-manager.h"
#include "core/util-lib.h"

namespace troll {

void SceneManager::SetupScene(const Scene& scene) {
  scene_ = scene;
  renderer_.ClearScreen();
  renderer_.FillColour(scene_.bitmap_config().background_colour(),
                       scene_.viewport());

  if (scene_.bitmap_config().has_bitmap()) {
    renderer_.BlitTexture(
        ResourceManager::Instance().GetTexture(scene_.bitmap_config().bitmap()),
        Box(), Box());
  }

  CollisionChecker::Instance().Init();
  for (const auto& node : scene_.scene_node()) {
    AddSceneNode(node);
  }

  for (const auto& action : scene_.on_init()) {
    ActionManager::Instance().Execute(action);
  }
}

void SceneManager::AddSceneNode(const SceneNode& node) {
  const auto res = scene_nodes_.emplace(node.id(), node);
  LOG_IF(ERROR, !res.second) << "AddSceneNode() SceneNode with id='"
                             << node.id() << "' already exists.";

  const auto it = res.first;
  Dirty(it->second);
}

void SceneManager::RemoveSceneNode(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  LOG_IF(ERROR, it == scene_nodes_.end())
      << "RemoveSceneNode() cannot find SceneNode with id='" << id << "'.";

  dirty_boxes_.push_back(GetSceneNodeBoundingBox(it->second));
  dead_scene_nodes_.push_back(id);
}

SceneNode* SceneManager::GetSceneNodeById(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  return it != scene_nodes_.end() ? &it->second : nullptr;
}

SceneNode* SceneManager::GetSceneNodeAt(const Vector& at) { return nullptr; }

Box SceneManager::GetSceneNodeBoundingBox(const SceneNode& node) const {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  auto bounding_box = sprite.film(node.frame_index());
  bounding_box.set_left(node.position().x());
  bounding_box.set_top(node.position().y());
  return bounding_box;
}

void SceneManager::SetViewport(const Box& view) {
  viewport_ = view;
  // TODO(bourdenas): Render everything.
}

void SceneManager::ScrollViewport(const Vector& by) {
  // TODO(bourdenas): translate(viewport, by)
  // TODO(bourdenas): Render everything.
}

void SceneManager::Render() {
  CleanUpDeletedSceneNodes();

  for (const auto& box : dirty_boxes_) {
    renderer_.FillColour(scene_.bitmap_config().background_colour(), box);
  }
  dirty_boxes_.clear();

  for (const auto* node : dirty_nodes_) {
    BlitSceneNode(*node);
  }
  dirty_nodes_.clear();

  renderer_.Flip();
}

void SceneManager::Dirty(const SceneNode& scene_node) {
  dirty_boxes_.push_back(GetSceneNodeBoundingBox(scene_node));
  dirty_nodes_.push_back(&scene_node);
  CollisionChecker::Instance().Dirty(scene_node);
}

void SceneManager::BlitSceneNode(const SceneNode& node) const {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  const auto& bounding_box = sprite.film(node.frame_index());
  Box destination = bounding_box;
  destination.set_left(node.position().x());
  destination.set_top(node.position().y());

  renderer_.BlitTexture(
      ResourceManager::Instance().GetTexture(sprite.resource()), bounding_box,
      destination);
}

void SceneManager::CleanUpDeletedSceneNodes() {
  for (const auto& id : dead_scene_nodes_) {
    const auto it = scene_nodes_.find(id);
    LOG_IF(ERROR, it == scene_nodes_.end())
        << "CleanUpDeletedSceneNodes() SceneNode with id='" << id
        << "' was not found.";
    AnimatorManager::Instance().StopNodeAnimations(id);

    const auto dirty_it =
        std::remove(dirty_nodes_.begin(), dirty_nodes_.end(), &it->second);
    if (dirty_it != dirty_nodes_.end()) {
      dirty_nodes_.erase(dirty_it, dirty_nodes_.end());
    }
    scene_nodes_.erase(it);
  }
  dead_scene_nodes_.clear();
}

}  // namespace troll
