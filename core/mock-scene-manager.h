#ifndef TROLL_CORE_MOCK_SCENE_MANAGER_H_
#define TROLL_CORE_MOCK_SCENE_MANAGER_H_

#include <unordered_map>

#include "core/scene-manager.h"

namespace troll {

class MockSceneManager : public SceneManager {
 public:
  MockSceneManager() : SceneManager(Renderer()) {}

  SceneNode* GetSceneNodeById(const std::string& id) override {
    const auto it = get_scene_node_by_id_cache_.find(id);
    return it != get_scene_node_by_id_cache_.end() ? it->second : nullptr;
  }

  Box SceneManager::GetSceneNodeBoundingBox(
      const SceneNode& node) const override {
    const auto it = get_scene_node_bounding_box_cache_.find(node.id());
    return it != get_scene_node_bounding_box_cache_.end() ? it->second
                                                          : nullptr;
  }

  void Dirty(const SceneNode& scene_node) override {}

  void AddMockSceneNode(SceneNode* node) {
    get_scene_node_by_id_cache_[node->id()] = node;
  }
  void RemoveMockSceneNode(const std::string& id) {
    get_scene_node_by_id_cache_.erase(id);
  }

  void AddMockSceneNodeBoundingBox(const std::string& id,
                                   const Box& bounding_box) {
    get_scene_node_bounding_box_cache_[id] = bounding_box;
  }

 private:
  // SceneNodes are not owned by this map.
  std::unordered_map<std::string, SceneNode*> get_scene_node_by_id_cache_;

  std::unordered_map<std::string, Box> get_scene_node_bounding_box_cache_;
};

}  // namespace troll

#endif  // TROLL_CORE_MOCK_SCENE_MANAGER_H_
