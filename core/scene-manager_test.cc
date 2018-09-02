#include "core/scene-manager.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "animation/animator-manager.h"
#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

using testing::Pointee;

class SceneManagerIntegrationTest : public testing::Test {
 protected:
  void SetUp() override {
    scene_manager_ = new SceneManager(Renderer());
    Core::Instance().SetupTestSceneManager(scene_manager_);

    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: 'sprite_a'
      film { width: 10  height: 10 }
      film { width: 20  height: 20 }
      film { width: 30  height: 30 })"));
  }

  void TearDown() override {
    AnimatorManager::Instance().StopAll();
    ResourceManager::Instance().CleanUp();
  }

  SceneManager* scene_manager_ = nullptr;
};

// Running an animation script on non-existing scene node is a noop.
TEST_F(SceneManagerIntegrationTest, RunScriptOnNonExistingSceneNode) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(5);

  EXPECT_EQ(scene_manager_->GetSceneNodeById("node_a"), nullptr);
}

// Running a non-existing animation script on an existing scene node causes
// death.
TEST_F(SceneManagerIntegrationTest, RunNonExistingScriptOnSceneNode) {
  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  EXPECT_DEATH(AnimatorManager::Instance().Play("script_a", "node_a"),
               "AnimationScript with id=");
}

// Running an non-terminating animation script on a scene node, applies its
// effects perpetually.
TEST_F(SceneManagerIntegrationTest, RunIndefiniteScriptAnimation) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(12);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  AnimatorManager::Instance().Progress(5);

  // Animation will never stop executing by iteself.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 3  y: 0  z: 0
                })"))));
}

// Animations with specified number of repetitions do not have an effect after
// they are done.
TEST_F(SceneManagerIntegrationTest, RunFiniteScriptAnimation) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
        repeat: 2
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(20);

  // Animation executes only twice, even though it could have run 4 times if it
  // wasn't for its limit.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));
}

// Animations scripts have no longer an effect after they are stopped.
TEST_F(SceneManagerIntegrationTest, RunAndStopIndefiniteScriptAnimation) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(12);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  AnimatorManager::Instance().Stop("script_a", "node_a");
  AnimatorManager::Instance().Progress(5);

  // Animation stopped and no longer executes.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));
}

// Pausing an animation script halts temporarily its effects and are resumed
// when it resumes itself.
TEST_F(SceneManagerIntegrationTest, RunPauseResumeFiniteScriptAnimation) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
        repeat: 3
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(12);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  AnimatorManager::Instance().Pause("script_a", "node_a");
  AnimatorManager::Instance().Progress(5);

  // Animation paused and doesn't run until resumed.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  AnimatorManager::Instance().Resume("script_a", "node_a");
  AnimatorManager::Instance().Progress(10);

  // Animation resumes and runs one more time before it self-terminates.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 3  y: 0  z: 0
                })"))));
}

// Halting all animation scripts for a specific scene node has the intended
// effects and do not affect other nodes.
TEST_F(SceneManagerIntegrationTest, StopMultipleNodeAnimations) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_b'
    animation {
      translation {
        vec { y: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));
  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Play("script_b", "node_a");
  AnimatorManager::Instance().Play("script_a", "node_b");
  AnimatorManager::Instance().Progress(10);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 2  z: 0
                })"))));
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_b"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_b'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  AnimatorManager::Instance().StopNodeAnimations("node_a");
  AnimatorManager::Instance().Progress(5);

  // Animations don't run anymore for node_a.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 2  z: 0
                })"))));
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_b"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_b'
                sprite_id: 'sprite_a'
                position {
                  x: 3  y: 0  z: 0
                })"))));
}

// Stopping animation script for non-existing scene node is safe with no effect.
TEST_F(SceneManagerIntegrationTest, StopNodeAnimationsForNonExistingNode) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(10);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));
  EXPECT_EQ(scene_manager_->GetSceneNodeById("node_b"), nullptr);

  AnimatorManager::Instance().StopNodeAnimations("node_b");
  AnimatorManager::Instance().Progress(5);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 3  y: 0  z: 0
                })"))));
  EXPECT_EQ(scene_manager_->GetSceneNodeById("node_b"), nullptr);
}

// Applying a script twice on the same scene node doubles its effects.
TEST_F(SceneManagerIntegrationTest,
       ApplyingMultipleTimesScriptOnNodeMultipliesEffect) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(10);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 4  y: 0  z: 0
                })"))));
}

// Animation scripts applied on scene nodes that got destroyed are halted
// automatically and are safe for the system.
TEST_F(SceneManagerIntegrationTest, AnimationStopsWhenSceneNodeIsRemoved) {
  TestingResourceManager::SetTestAnimationScript(ParseProto<AnimationScript>(R"(
    id: 'script_a'
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a')"));

  AnimatorManager::Instance().Play("script_a", "node_a");
  AnimatorManager::Instance().Progress(10);

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_a"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a'
                sprite_id: 'sprite_a'
                position {
                  x: 2  y: 0  z: 0
                })"))));

  // Create a new scene manager as a hacky way to delete the 'test_node'.
  scene_manager_ = new SceneManager(Renderer());
  Core::Instance().SetupTestSceneManager(scene_manager_);

  AnimatorManager::Instance().Progress(5);
  EXPECT_EQ(scene_manager_->GetSceneNodeById("node_a"), nullptr);
}

}  // namespace troll
