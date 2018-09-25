#include "animation/script-animator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class AnimatorScriptTest : public testing::Test {
 protected:
  void SetUp() override {
    scene_manager_ = new SceneManager(Renderer());
    Core::Instance().SetupTestSceneManager(scene_manager_);

    // Setup dummy sprite for 'test_node' with 5 films that are need for the
    // frame range animations in the tests.
    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: ''
      film {} film {} film {} film {} film {})"));
  }

  SceneManager* scene_manager_ = nullptr;
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

TEST_F(AnimatorScriptTest, RunScriptWithSingleRepeatableAnimation) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 0  z:0 }
                )")));

  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(5));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 3  y: 0  z:0 }
                )")));
}

TEST_F(AnimatorScriptTest, RunScriptWithSingleOneOffAnimation) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
        repeat: 1
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_TRUE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 1  y: 0  z:0 }
                )")));

  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_TRUE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 1  y: 0  z:0 }
                )")));
}

TEST_F(AnimatorScriptTest, RunScriptWithSequencedAnimationThatDoesntFinish) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
        repeat: 2
      }
    }
    animation {
      frame_range {
        start_frame: 0
        end_frame: 3
        delay: 10
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(5));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 1  y: 0  z:0 }
                )")));

  EXPECT_FALSE(animator.Progress(5));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 0  z:0 }
                frame_index: 0
                )")));

  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(20));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 0  z:0 }
                frame_index: 2
                )")));

  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(20));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 0  z:0 }
                frame_index: 1
                )")));

  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());
}

TEST_F(AnimatorScriptTest,
       RunScriptWithSequencedCompositeAnimationThatFinishes) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1  y: 2 }
        delay: 5
        repeat: 2
      }
      frame_range {
        start_frame: 2
        end_frame: 5
        delay: 10
      }
    }
    animation {
      flash {
        delay: 2
        repeat: 2
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 4  z:0 }
                frame_index: 2
                )")));

  EXPECT_FALSE(animator.Progress(2));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 4  z:0 }
                frame_index: 2
                visible: false
                )")));

  EXPECT_TRUE(animator.Progress(2));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 4  z:0 }
                frame_index: 2
                visible: true
                )")));

  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_TRUE(animator.Progress(2));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 4  z:0 }
                frame_index: 2
                visible: true
                )")));

  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());
}

TEST_F(AnimatorScriptTest, RunRepeatedScript) {
  // Frame range animation is not repeatable, but the script itself is repeated
  // twice.
  const auto script = ParseProto<AnimationScript>(R"(
    repeat: 2
    animation {
      translation {
        vec { x: 1  y: 2 }
        delay: 10
        repeat: 3
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 1  y: 2  z: 0}
                )")));

  EXPECT_FALSE(animator.Progress(20));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 3  y: 6  z: 0}
                )")));

  EXPECT_TRUE(animator.Progress(30));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 6  y: 12  z: 0}
                )")));

  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_TRUE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 6  y: 12  z: 0}
                )")));

  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());
}

TEST_F(AnimatorScriptTest, RunScriptAnimationAndNodeIsDeleted) {
  const auto script = ParseProto<AnimationScript>(R"(
    animation {
      translation {
        vec { x: 1 }
        delay: 5
      }
    })");
  ScriptAnimator animator(script, "test_node");
  scene_manager_->AddSceneNode(ParseProto<SceneNode>("id: 'test_node'"));

  animator.Start();
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());
  EXPECT_FALSE(animator.is_paused());

  EXPECT_FALSE(animator.Progress(10));
  EXPECT_THAT(*scene_manager_->GetSceneNodeById("test_node"),
              EqualsProto(ParseProto<SceneNode>(R"(
                id: 'test_node'
                position { x: 2  y: 0  z:0 }
                )")));
  EXPECT_TRUE(animator.is_running());
  EXPECT_FALSE(animator.is_finished());

  // Create a new scene manager as a hacky way to delete the 'test_node'.
  scene_manager_ = new SceneManager(Renderer());
  Core::Instance().SetupTestSceneManager(scene_manager_);

  EXPECT_EQ(scene_manager_->GetSceneNodeById("test_node"), nullptr);
  EXPECT_TRUE(animator.Progress(10));
  EXPECT_FALSE(animator.is_running());
  EXPECT_TRUE(animator.is_finished());
}

}  // namespace troll
