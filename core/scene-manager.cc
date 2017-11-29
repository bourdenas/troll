#include "core/scene-manager.h"

#include "core/action-manager.h"
#include "core/collision-checker.h"
#include "core/troll-core.h"
#include "core/util-lib.h"

namespace troll {

void SceneManager::SetupScene(const Scene& scene) {
  scene_ = scene;
  renderer_.ClearScreen();
  renderer_.FillColour(scene_.bitmap_config().background_colour(),
                       scene_.viewport());

  if (scene_.bitmap_config().has_bitmap()) {
    renderer_.BlitTexture(
        *Core::Instance().GetTexture(scene_.bitmap_config().bitmap()), Box(),
        Box());
  }

  CollisionChecker::Instance().Init(scene_);
  for (const auto& node : scene_.scene_node()) {
    AddSceneNode(node);
  }

  for (const auto& action : scene_.on_init()) {
    ActionManager::Instance().Execute(action);
  }
}

void SceneManager::AddSceneNode(const SceneNode& node) {
  const auto it = scene_nodes_.emplace(node.id(), node).first;
  Dirty(it->second);

  CollisionChecker::Instance().AddSceneNode(it->second);
}

void SceneManager::RemoveSceneNode(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  Dirty(it->second);

  CollisionChecker::Instance().RemoveSceneNode(it->second);
  dirty_nodes_.erase(
      std::remove(dirty_nodes_.begin(), dirty_nodes_.end(), &it->second));
  scene_nodes_.erase(it);
}

SceneNode* SceneManager::GetSceneNodeById(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  return it != scene_nodes_.end() ? &it->second : nullptr;
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
  dirty_boxes_.push_back(util::GetSceneNodeBoundingBox(scene_node));
  dirty_nodes_.push_back(&scene_node);
  CollisionChecker::Instance().Dirty(scene_node);
}

void SceneManager::BlitSceneNode(const SceneNode& node) const {
  const auto* sprite = Core::Instance().GetSprite(node.sprite_id());
  if (sprite == nullptr) return;

  const auto& bounding_box = sprite->film(node.frame_index());
  Box destination = bounding_box;
  destination.set_left(node.position().x());
  destination.set_top(node.position().y());

  renderer_.BlitTexture(*Core::Instance().GetTexture(sprite->resource()),
                        bounding_box, destination);
}

}  // namespace troll
