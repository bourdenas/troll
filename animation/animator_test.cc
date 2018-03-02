#include "animation/animator.h"

#include <google/protobuf/text_format.h>
#include <gtest/gtest.h>

#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {

TEST(AnimatorTest, TestTrivial) {
  Animation animation;
  google::protobuf::TextFormat::ParseFromString(R"(
    repeat: 1
    translation {
      vec {
        y: -2
      }
      delay: 35
    })",
                                                &animation);

  SceneNode scene_node;

  Animator animator(animation);
  animator.Start();

  // Not done after 10 msec.
  EXPECT_TRUE(animator.Progress(10, &scene_node));

  // After 30 more msec (40 msec in total) the animator is done.
  EXPECT_FALSE(animator.Progress(30, &scene_node));
}

}  // namespace troll