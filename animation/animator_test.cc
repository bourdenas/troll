#include "animation/animator.h"

#include <gmock/gmock.h>
#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>

#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"

namespace troll {

class AnimatorTest : public testing::Test {
 protected:
  virtual void SetUp() {
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

TEST_F(AnimatorTest, IndefiniteAnimation) {
  Animator animator(animation_);
  animator.Start();

  // Not executed after 5 msec, animation will be performed after 10 msec.
  EXPECT_TRUE(animator.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // After 10 msec (15 msec in total) the animator is executed but does not end
  // since it is indefinite.
  EXPECT_TRUE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -2 z: 0
    })")));

  EXPECT_TRUE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -4 z: 0
    })")));
}

TEST_F(AnimatorTest, AnimationPerformedOnlyOnce) {
  animation_.set_repeat(1);
  Animator animator(animation_);
  animator.Start();

  // Not executed after 5 msec, animation will be performed after 10 msec.
  EXPECT_TRUE(animator.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // After 5 more msec (10 msec in total) the animator is executed and finishes.
  EXPECT_FALSE(animator.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -2 z: 0
    })")));
}

TEST_F(AnimatorTest, AnimationPerformedThreeTimes) {
  animation_.set_repeat(3);
  Animator animator(animation_);
  animator.Start();

  EXPECT_TRUE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -2 z: 0
    })")));

  EXPECT_TRUE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -4 z: 0
    })")));

  EXPECT_FALSE(animator.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 0 y: -6 z: 0
    })")));

}

}  // namespace troll
