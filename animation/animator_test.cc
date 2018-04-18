#include "animation/animator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"

namespace troll {

class AnimatorTest : public testing::Test {
 protected:
  void SetUp() override {
    animation_ = ParseProto<Animation>(R"(
      translation {
        vec {
          y: -2
        }
        delay: 10
      })");
  }

  Animation animation_;
  SceneNode scene_node_;
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
  Animator animator(composite_animation);
  animator.Start(&scene_node_);
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
  Animator animator(composite_animation);
  animator.Start(&scene_node_);
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
