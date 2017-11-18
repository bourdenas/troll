#include "core/scene-manager.h"

#include "core/troll-core.h"

#include <iostream>

namespace troll {

void SceneManager::SetupScene() {
  renderer_.ClearScreen();
  if (scene_.bitmap_config().has_bitmap()) {
    renderer_.BlitTexture(
        *Core::Instance().textures_[scene_.bitmap_config().bitmap()], Box(),
        Box());
  }
}

void SceneManager::SetViewport(const Box& view) {
  viewport_ = view;
  Invalidate(viewport_);
}

void SceneManager::ScrollViewport(const Vector& by) {
  // TODO: translate(viewport, by)
  Invalidate(viewport_);
}

void SceneManager::Render() { renderer_.Flip(); }

void SceneManager::Invalidate(const Box& region) {
  dirty_boxes_.push_back(region);
}

}  // namespace troll
