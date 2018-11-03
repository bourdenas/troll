#include "animation/performer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/scene-manager.h"
#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class PerformerTest : public testing::Test {
 protected:
  void SetUp() override {
    scene_manager_ = new SceneManager();
    Core::Instance().SetupTestSceneManager(scene_manager_);

    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: ''
      film{} film{} film{} film{} film{} film{} film{})"));
  }

  void TearDown() override { ResourceManager::Instance().CleanUp(); }

  SceneNode scene_node_;
  SceneManager* scene_manager_ = nullptr;
};

TEST_F(PerformerTest, Translation) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    repeat: 1
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Not enough time for translation.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Translation happend and animation finishes.
  EXPECT_TRUE(performer.Progress(3, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1  y: 0  z: 0
    })")));
}

TEST_F(PerformerTest, TranslationIndefinite) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 5
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Not enough time for translation.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // First translation happens but performer is not finished.
  EXPECT_FALSE(performer.Progress(3, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1  y: 0  z: 0
    })")));

  // Translation never finishes.
  for (int i = 0; i < 10; ++i) {
    EXPECT_FALSE(performer.Progress(5, &scene_node_));
  }
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 11  y: 0  z: 0
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

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3  y: 0  z: 0
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

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(15, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3  y: 0  z: 0
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

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(50, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3  y: 0  z: 0
    })")));
}

TEST_F(PerformerTest, TranslationOneOffWithoutDelay) {
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 0
    repeat: 1
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1  y: 0  z: 0
    })")));
}

TEST_F(PerformerTest, TranslationRepeatableWithoutDelay) {
  // This is an odd case, where animation is applied instantly repeatadly.
  // The expected behaviour is that the animation is applied only once per call
  // of Progress() and not cause infinite loop. However, this is not a reliable
  // way to apply an animation since it will not be deterministic in its
  // effects, but will vary based on frame-rate.
  const auto translation = ParseProto<VectorAnimation>(R"(
    delay: 0
    vec {
      x: 1
    })");
  TranslationPerformer performer(translation);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 1  y: 0  z: 0
    })")));
  EXPECT_FALSE(performer.Progress(20, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 2  y: 0  z: 0
    })")));
  EXPECT_FALSE(performer.Progress(1, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 3  y: 0  z: 0
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

  performer.Start(&scene_node_);
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

  performer.Start(&scene_node_);
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

  performer.Start(&scene_node_);
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

TEST_F(PerformerTest, FrameList) {
  const auto frame_list = ParseProto<FrameListAnimation>(R"(
    frame: [0, 2, 4, 6]
    delay: 5
    repeat: 1
    )");
  FrameListPerformer performer(frame_list);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    )")));

  EXPECT_TRUE(performer.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 6
    )")));
}

TEST_F(PerformerTest, FrameListWithSingleFrame) {
  const auto frame_list = ParseProto<FrameListAnimation>(R"(
    frame: [0]
    delay: 5
    repeat: 1
    )");
  FrameListPerformer performer(frame_list);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));
}

TEST_F(PerformerTest, FrameListRewindsOnRepeat) {
  const auto frame_list = ParseProto<FrameListAnimation>(R"(
    frame: [0, 2, 4, 6]
    delay: 5
    repeat: 2
    )");
  FrameListPerformer performer(frame_list);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 0
    )")));

  EXPECT_FALSE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    )")));

  EXPECT_FALSE(performer.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 6
    )")));

  EXPECT_FALSE(performer.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 2
    )")));

  EXPECT_TRUE(performer.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    frame_index: 6
    )")));
}

TEST_F(PerformerTest, Flash) {
  const auto flash = ParseProto<FlashAnimation>(R"(
    delay: 5
    repeat: 1
    )");
  FlashPerformer performer(flash);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(5, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    visible: false
    )")));
}

TEST_F(PerformerTest, FlashRepeats) {
  const auto flash = ParseProto<FlashAnimation>(R"(
    delay: 5
    repeat: 2
    )");
  FlashPerformer performer(flash);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(15, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    visible: true
    )")));
}

TEST_F(PerformerTest, Goto) {
  const auto goto_animation = ParseProto<GotoAnimation>(R"(
    destination {
      x: 10  y: 5  z: 0
    }
    step: 2
    delay: 1
    )");
  GotoPerformer performer(goto_animation);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  // Math with double is hard! Not easy to check intermediate progress of
  // position.
  EXPECT_FALSE(performer.Progress(2, &scene_node_));
  EXPECT_LT(3, scene_node_.position().x());
  EXPECT_LT(1.5, scene_node_.position().y());
  EXPECT_EQ(0, scene_node_.position().z());

  // In final destination, we don't rely on double math, instead hard code the
  // final destination.
  EXPECT_TRUE(performer.Progress(10, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 10  y: 5  z: 0
    })")));
}

TEST_F(PerformerTest, GotoAlreadyThere) {
  const auto goto_animation = ParseProto<GotoAnimation>(R"(
    destination {
      x: 10  y: 5  z: 0
    }
    step: 2
    delay: 1
    )");
  GotoPerformer performer(goto_animation);

  scene_node_ = ParseProto<SceneNode>(R"(
    position {
      x: 10  y: 5  z: 0
    })");
  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 10  y: 5  z: 0
    })")));

  EXPECT_TRUE(performer.Progress(2, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
    position {
      x: 10  y: 5  z: 0
    })")));
}

TEST_F(PerformerTest, Timer) {
  const auto timer = ParseProto<TimerAnimation>(R"(
    delay: 1000
    )");
  TimerPerformer performer(timer);

  performer.Start(&scene_node_);
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_FALSE(performer.Progress(500, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));

  EXPECT_TRUE(performer.Progress(500, &scene_node_));
  EXPECT_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>("")));
}

}  // namespace troll
