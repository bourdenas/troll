#include "core/scene-manager.h"

#include <boost/geometry/algorithms/transform.hpp>

#include "core/primitives.h"

namespace troll {

void SceneManager::SetupScene(std::unique_ptr<Scene> scene) {
  scene_ = std::move(scene);
}

void SceneManager::UnloadScene() {}

void SceneManager::SetViewport(const Box& view) {
  viewport_ = view;
  Invalidate({{0, 0}, viewport_.max_corner()});
}

void SceneManager::ScrollViewport(const Point& by) {
  // TODO: translate(viewport, by)
  Invalidate(viewport_);
}

void SceneManager::Invalidate(const Box& region) {
  dirty_boxes_.push_back(region);
}

}  // namespace troll
