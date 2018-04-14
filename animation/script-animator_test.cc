#include "animation/script-animator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/scene-manager.h"
#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"

namespace troll {

using testing::Return;

class MockSceneManager : public SceneManager {
 public:
  MockSceneManager() : SceneManager(Renderer()) {}

  SceneNode* GetSceneNodeById(const std::string& id) override {
    const auto it = get_scene_node_by_id_cache_.find(id);
    return it != get_scene_node_by_id_cache_.end() ? &it->second : nullptr;
  }

  void Dirty(const SceneNode& scene_node) override {}

  void AddMockSceneNode(const std::string& id, const SceneNode& node) {
    get_scene_node_by_id_cache_[id] = node;
  }

 private:
  std::unordered_map<std::string, SceneNode> get_scene_node_by_id_cache_;
};

class AnimatorScriptTest : public testing::Test {
 protected:
  virtual void SetUp() {
    scene_manager_ = new MockSceneManager;
    Core::Instance().SetupTestSceneManager(scene_manager_);
  }

  MockSceneManager* scene_manager_ = nullptr;
};

TEST_F(AnimatorScriptTest, RunScriptOnNonExistingSceneNode) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })");
  ScriptAnimator animator(script, "test_node");

  animator.Start();
  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());
}

TEST_F(AnimatorScriptTest, RunScriptWithSingleAnimation) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })");
  ScriptAnimator animator(script, "test_node");

  SceneNode test_node;
  scene_manager_->AddMockSceneNode("test_node", test_node);

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());
}

}  // namespace troll
