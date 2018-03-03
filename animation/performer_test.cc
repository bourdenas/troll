#include "animation/performer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"

namespace troll {

class PerformerTest : public testing::Test {
 protected:
  virtual void SetUp() {}

  SceneNode scene_node_;
};

TEST_F(PerformerTest, Translation) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    repeat: 1
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  // Init is a noop.
  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Not enough time for translation.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Translation happend and animation finishes.
  EXPECT_TRUE(performer.Progress(3, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1 y: 0 z: 0
    })")));
}

TEST_F(PerformerTest, TranslationIndefinite) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  // Init is a noop.
  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Not enough time for translation.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // First translation happens but performer is not finished.
  EXPECT_FALSE(performer.Progress(3, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1 y: 0 z: 0
    })")));

  // Translation never finishes.
  for (int i = 0; i < 10; ++i) {
    EXPECT_FALSE(performer.Progress(5, &scene_node_));
  }
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 11 y: 0 z: 0
    })")));
}

TEST_F(PerformerTest, TranslationRepeatableThreeTimes) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    repeat: 3
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  // Init is a noop.
  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3 y: 0 z: 0
    })")));
}

TEST_F(PerformerTest, TranslationProgressesMultipleTimesIfNeeded) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    repeat: 3
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  // Init is a noop.
  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(15, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3 y: 0 z: 0
    })")));
}

TEST_F(PerformerTest, TranslationDoesntProgressMoreThanItIsAllowed) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    repeat: 3
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  // Init is a noop.
  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(50, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3 y: 0 z: 0
    })")));
}

TEST_F(PerformerTest, FrameRange) {
  const auto frame_range = ParseProto<FrameRangeAnimation>(R"(
    start_frame: 0
    end_frame: 3
    delay: 5
    repeat: 1
    )");
  FrameRangePerformer performer(frame_range);

  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  // Not enough time for progress.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_FALSE(performer.Progress(3, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 1
    )")));

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    )")));
}

TEST_F(PerformerTest, FrameRangeWithSingleFrame) {
  const auto frame_range = ParseProto<FrameRangeAnimation>(R"(
    start_frame: 0
    end_frame: 1
    delay: 5
    repeat: 1
    )");
  FrameRangePerformer performer(frame_range);

  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));
}

TEST_F(PerformerTest, FrameRangeRewindsOnRepeat) {
  const auto frame_range = ParseProto<FrameRangeAnimation>(R"(
    start_frame: 0
    end_frame: 3
    delay: 5
    repeat: 2
    )");
  FrameRangePerformer performer(frame_range);

  performer.Init(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_EQ(1, scene_node_.frame_index());

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_EQ(2, scene_node_.frame_index());

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_EQ(0, scene_node_.frame_index());

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_EQ(1, scene_node_.frame_index());

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_EQ(2, scene_node_.frame_index());
}

}  // namespace troll
