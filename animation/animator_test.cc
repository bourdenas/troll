#include "animation/animator.h"

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/scene-manager.h"
#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class AnimatorTest : public testing::Test {
 protected:
  void SetUp() override {
    scene_manager_ = new SceneManager(Renderer());
    Core::Instance().SetupTestSceneManager(scene_manager_);

    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: ''
      film{} film{} film{})"));

    animation_ = ParseProto<Animation>(R"(
      translation {
        vec {
          y: -2
        }
        delay: 10
      })");
  }

  void TearDown() override { ResourceManager::Instance().CleanUp(); }

  Animation animation_;
  SceneNode scene_node_;
  SceneManager* scene_manager_ = nullptr;
};

TEST_F(AnimatorTest, CompositeAnimationTerminatesWhenAnyPartTerminates) {
  const auto composite_animation = ParseProto<Animation>(R"(
    translation {
      vec { x: 1 }
      delay: 5
    }
    frame_range {
      start_frame: 0
      end_frame: 3
      delay: 10
      repeat: 2
    })");

  Animator animator;
  animator.Start(composite_animation, &scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  // The FrameRange will be exectuted twice (60 msec in total) so the
  // Translation will be executed 12 times till then.
  EXPECT_FALSE(animator.Progress(20, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    position {
      x: 4 y: 0 z: 0
    })")));
  EXPECT_TRUE(animator.Progress(30, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    position {
      x: 10 y: 0 z: 0
    })")));
}

TEST_F(AnimatorTest, CompositeAnimationTerminatesWhenAllPartsTerminate) {
  const auto composite_animation = ParseProto<Animation>(R"(
    termination: ALL
    translation {
      vec { x: 1 }
      delay: 5
      repeat: 1
    }
    frame_range {
      start_frame: 0
      end_frame: 3
      delay: 10
      repeat: 1
    })");

  Animator animator;
  animator.Start(composite_animation, &scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  // The translation is done in its first execution. It is no longer applied but
  // the frame_range is still running. Translation will be executed 12 times
  // till then.
  EXPECT_FALSE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 1
    position {
      x: 1 y: 0 z: 0
    })")));
  EXPECT_TRUE(animator.Progress(30, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    position {
      x: 1 y: 0 z: 0
    })")));
}

TEST_F(AnimatorTest, NonRepeatableAnimationTerminatesCompositeAnimation) {
  const auto composite_animation = ParseProto<Animation>(R"(
    translation {
      vec { x: 1 }
      delay: 5
    }
    frame_range {
      start_frame: 0
      end_frame: 3
      delay: 10
    }
    timer {
     delay: 25 
    })");

  Animator animator;
  animator.Start(composite_animation, &scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  // Timer is non-repeatable so after its delay it will cause the composite
  // animation to finish.
  EXPECT_TRUE(animator.Progress(25, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    position {
      x: 5 y: 0 z: 0
    })")));
}

}  // namespace troll
