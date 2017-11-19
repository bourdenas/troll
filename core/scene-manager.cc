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
}

void SceneManager::AddSceneNode(SceneNode* node) {}

void SceneManager::RemoveSceneNode(const std::string& id) {}

void SceneManager::SetViewport(const Box& view) {
  viewport_ = view;
  // TODO: Render everything.
}

void SceneManager::ScrollViewport(const Vector& by) {
  // TODO: translate(viewport, by)
  // TODO: Render everything.
}

void SceneManager::Render() { renderer_.Flip(); }

void SceneManager::Dirty(const SceneNode* scene_node) {
  dirty_nodes_.push_back(scene_node);
}

}  // namespace troll
