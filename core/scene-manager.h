#ifndef TROLL_CORE_SCENE_MANAGER_H_
#define TROLL_CORE_SCENE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "proto/primitives.pb.h"
#include "proto/scene.pb.h"
#include "sdl/renderer.h"

namespace troll {

class SceneManager {
 public:
  SceneManager(const Renderer& renderer) : renderer_(renderer) {}
  ~SceneManager() = default;

  void SetupScene(const Scene& scene);

  void AddSceneNode(const SceneNode& node);
  void RemoveSceneNode(const std::string& id);

  SceneNode* GetSceneNodeById(const std::string& id);
  SceneNode* GetSceneNodeAt(const Vector& at);

  void SetViewport(const Box& view);
  void ScrollViewport(const Vector& by);

  void Render();

  // Marks a scene node that needs to be queued for rendering during this frame.
  void Dirty(const SceneNode& scene_node);

  const Scene& scene() const { return scene_; }
  const Box& viewport() const { return viewport_; }

 private:
  void BlitSceneNode(const SceneNode& node) const;
  void CleanUpDeletedSceneNodes();

  const Renderer& renderer_;

  Scene scene_;
  Box world_bounds_;
  Box viewport_;

  std::unordered_map<std::string, SceneNode> scene_nodes_;
  std::vector<std::string> dead_scene_nodes_;

  std::vector<const SceneNode*> dirty_nodes_;
  std::vector<Box> dirty_boxes_;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_MANAGER_H_
