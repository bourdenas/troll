#include "core/scene-manager.h"

#include "core/troll-core.h"

namespace troll {

void SceneManager::SetupScene() {
  renderer_.ClearScreen();
  if (scene_.bitmap_config().has_bitmap()) {
    renderer_.BlitTexture(
        *Core::Instance().textures_[scene_.bitmap_config().bitmap()], Box(),
        Box());
  }

  for (const auto& node : scene_.scene_node()) {
    AddSceneNode(node);
  }
}

void SceneManager::AddSceneNode(const SceneNode& node) {
  const auto& sprite = Core::Instance().sprites_[node.sprite_id()];
  const auto& bounding_box = sprite.film(node.frame_index());

  Box destination;
  destination.set_left(node.position().x());
  destination.set_top(node.position().y());
  destination.set_width(bounding_box.width());
  destination.set_height(bounding_box.height());

  renderer_.BlitTexture(*Core::Instance().textures_[sprite.resource()],
                        bounding_box, destination);

  scene_nodes_.emplace(node.id(), node);
}

void SceneManager::RemoveSceneNode(const std::string& id) {
  // TODO(bourdenas): This will create dangling ptrs all over the place.
  scene_nodes_.erase(id);
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

void SceneManager::Render() { renderer_.Flip(); }

void SceneManager::Dirty(const SceneNode* scene_node) {
  dirty_nodes_.push_back(scene_node);
}

}  // namespace troll
