#ifndef TROLL_CORE_SCENE_MANAGER_H_
#define TROLL_CORE_SCENE_MANAGER_H_

#include <memory>
#include <string>

#include "core/scene.h"
#include "proto/primitives.pb.h"
#include "sdl/renderer.h"

namespace troll {

class SceneManager {
 public:
  SceneManager(const Scene& scene, const Renderer& renderer)
      : scene_(scene), renderer_(renderer) {}
  ~SceneManager() = default;

  void SetupScene();

  void AddSceneNode(SceneNode* node);
  void RemoveSceneNode(const std::string& id);

  const SceneNode& GetSceneNodeById(const std::string& id) const;
  const SceneNode& GetSceneNodeAt(const Vector& at) const;

  void SetViewport(const Box& view);
  void ScrollViewport(const Vector& by);

  void Render();

  // Marks a region inside the viewport dirty and queue it for rendering.
  void Invalidate(const Box& region);

  const Scene& scene() const { return scene_; }
  const Box& viewport() const { return viewport_; }

 private:
  Scene scene_;
  const Renderer& renderer_;

  Box world_bounds_;
  Box viewport_;

  std::vector<Box> dirty_boxes_;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_MANAGER_H_
