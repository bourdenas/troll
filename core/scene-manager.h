#ifndef TROLL_CORE_SCENE_MANAGER_H_
#define TROLL_CORE_SCENE_MANAGER_H_

#include <memory>
#include <string>

#include "core/bitmap.h"
#include "core/primitives.h"
#include "core/scene.h"

namespace troll {

class SceneManager {
 public:
  SceneManager() = default;
  ~SceneManager() = default;

  void SetupScene(std::unique_ptr<Scene> scene);
  void UnloadScene();

  void AddSceneNode(SceneNode* node);
  void RemoveSceneNode(const std::string& id);

  const SceneNode& GetSceneNodeById(const std::string& id) const;
  const SceneNode& GetSceneNodeAt(const Point& at) const;

  void SetViewport(const Box& view);
  void ScrollViewport(const Point& by);

  // Renders the scene on a bitmap.
  void Render(Bitmap* dst);

  // Marks a region inside the viewport dirty and queue it for rendering.
  void Invalidate(const Box& region);

  const Scene& scene() const { return *scene_; }
  const Box& viewport() const { return viewport_; }

 private:
  std::unique_ptr<Scene> scene_;

  Box world_bounds_;
  Box viewport_;

  std::vector<Box> dirty_boxes_;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_MANAGER_H_
