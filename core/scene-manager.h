#ifndef TROLL_CORE_SCENE_MANAGER_H_
#define TROLL_CORE_SCENE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>

#include "core/geometry.h"
#include "proto/primitives.pb.h"
#include "proto/scene.pb.h"
#include "sdl/renderer.h"

namespace troll {

class SceneManager {
 public:
  SceneManager(const Renderer& renderer) : renderer_(renderer) {}

  void SetupScene(const Scene& scene);

  void AddSceneNode(const SceneNode& node);
  void RemoveSceneNode(const std::string& id);

  // Marks a scene node that needs to be queued for rendering during this frame.
  void Dirty(const SceneNode& scene_node);

  const SceneNode* GetSceneNodeById(const std::string& id) const;
  SceneNode* GetSceneNodeById(const std::string& id);

  // Returns a view of active SceneNodes.
  auto GetSceneNodes() const { return scene_nodes_ | ranges::view::values; }

  // Returns a view of active SceneNodes that contain the point |at|. This is an
  // O(N) operation to the number of ScenNodes.
  std::vector<std::string> GetSceneNodesAt(const Vector& at) const;

  // Returns a view of active SceneNodes of a specific sprite. This is an O(N)
  // operation to the number of ScenNodes.
  std::vector<std::string> GetSceneNodesBySpriteId(
      const std::string& sprite_id) const;

  // Returns a view of active SceneNodes that match partially the ScenNode
  // described in the |pattern|. This is an O(N) operation to the number of
  // ScenNodes.
  std::vector<std::string> GetSceneNodesByPattern(
      const SceneNode& pattern) const;

  // Returns a view of SceneNodes references in |node_ids| that match partially
  // the ScenNode described in the |pattern|. This is an O(N) operation to the
  // number of |node_ids|.
  std::vector<std::string> GetSceneNodesByPattern(
      const SceneNode& pattern, const std::vector<std::string>& node_ids) const;

  void SetViewport(const Box& view);
  void ScrollViewport(const Vector& by);

  void Render();

  const Scene& scene() const { return scene_; }
  const Box& viewport() const { return viewport_; }

  // Returns a bounding box describing the position of input node in the scene.
  static Box GetSceneNodeBoundingBox(const SceneNode& node);

 private:
  void BlitSceneNode(const SceneNode& node) const;
  void CleanUpDeletedSceneNodes();

  // Returns true if |node| matches all fields present in the |pattern|.
  static bool NodePatternMatching(const SceneNode& pattern,
                                  const SceneNode& node);

  const Renderer& renderer_;

  Scene scene_;
  Box world_bounds_;
  Box viewport_;

  std::unordered_map<std::string, SceneNode> scene_nodes_;
  std::unordered_set<std::string> dead_scene_nodes_;

  std::vector<Box> dirty_boxes_;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_MANAGER_H_
