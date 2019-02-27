#include "animation/performer.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "core/scene-manager.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-core.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class PerformerFixture {
 public:
  PerformerFixture() {
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film{} film{} film{} film{} film{} film{} film{})"));

    core_.set_resource_manager(&resource_manager_);
    core_.set_scene_manager(&scene_manager_);

    scene_node_ = ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'");
  }

 protected:
  TestCore core_;
  ResourceManager resource_manager_;
  SceneManager scene_manager_ =
      SceneManager(&resource_manager_, nullptr, &core_);

  TestingResourceManager testing_resource_manager_ =
      TestingResourceManager(&resource_manager_);

  SceneNode scene_node_;
};

SCENARIO_METHOD(PerformerFixture, "Translation animation", "[performers]") {
  GIVEN("a translation animation") {
    auto translation = ParseProto<VectorAnimation>(R"(
        delay: 5
        vec { x: 1 })");

    WHEN("it runs indefinetely") {
      TranslationPerformer performer(translation);
      performer.Start(&scene_node_);

      AND_WHEN("not enought time passes") {
        REQUIRE_FALSE(performer.Progress(2, &scene_node_));

        THEN("it is a noop") {
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(
                                        "id: 'node_a' sprite_id: 'sprite_a'")));

          AND_WHEN("lots ot time passes") {
            REQUIRE_FALSE(performer.Progress(200, &scene_node_));

            THEN("the animation is applied and never finishes") {
              REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
                  id: 'node_a' sprite_id: 'sprite_a'
                  position { x: 40 y: 0 z: 0})")));
            }
          }
        }
      }
    }

    WHEN("it repeats only a number of times") {
      translation.set_repeat(3);
      TranslationPerformer performer(translation);
      performer.Start(&scene_node_);

      AND_WHEN("enought time passes") {
        REQUIRE(performer.Progress(15, &scene_node_));

        THEN("it terminates") {
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 3 y: 0 z: 0})")));
        }
      }

      AND_WHEN("a lot of time passes") {
        REQUIRE(performer.Progress(50, &scene_node_));

        THEN("it does not progress more than the required times") {
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 3 y: 0 z: 0})")));
        }
      }
    }

    WHEN("there is no animation delay") {
      translation.set_delay(0);

      AND_WHEN("it is an one-off") {
        translation.set_repeat(1);
        TranslationPerformer performer(translation);
        performer.Start(&scene_node_);

        THEN("no time needs to pass and animation executes and terminates") {
          REQUIRE(performer.Progress(0, &scene_node_));
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 1 y: 0 z: 0})")));
        }
      }

      AND_WHEN("it is an indefinite animation") {
        translation.set_repeat(0);
        TranslationPerformer performer(translation);
        performer.Start(&scene_node_);

        THEN("no matter how much time passes it executes once per call") {
          // This is an odd case, where animation is applied instantly
          // repeatedly. The expected behaviour is that the animation is applied
          // only once per call of Progress() and not cause infinite loop.
          // However, this is not a recommended setup for applying an animation
          // since it will not be deterministic in its effects, but will vary
          // based on frame-rate.
          REQUIRE_FALSE(performer.Progress(10, &scene_node_));
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 1 y: 0 z: 0})")));

          REQUIRE_FALSE(performer.Progress(30, &scene_node_));
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 2 y: 0 z: 0})")));

          REQUIRE_FALSE(performer.Progress(0, &scene_node_));
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 3 y: 0 z: 0})")));

          REQUIRE_FALSE(performer.Progress(100, &scene_node_));
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
              id: 'node_a' sprite_id: 'sprite_a'
              position { x: 4 y: 0 z: 0})")));
        }
      }
    }
  }
}

SCENARIO_METHOD(PerformerFixture, "FrameRange animation", "[performers]") {
  GIVEN("a frame-range animation") {
    auto frame_range = ParseProto<FrameRangeAnimation>(R"(
        start_frame: 0
        end_frame: 3
        delay: 5
        repeat: 2
        )");

    WHEN("it starts") {
      FrameRangePerformer performer(frame_range, &core_);
      performer.Start(&scene_node_);

      THEN("it immediately applies its first frame") {
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

        AND_WHEN("progress equals to delay is made") {
          REQUIRE_FALSE(performer.Progress(5, &scene_node_));

          THEN("the next frame is applied") {
            REQUIRE_THAT(
                scene_node_,
                EqualsProto(ParseProto<SceneNode>(
                    "id: 'node_a' sprite_id: 'sprite_a' frame_index: 1")));

            AND_WHEN("more progress is made") {
              REQUIRE_FALSE(performer.Progress(5, &scene_node_));

              THEN("the next frame is applied again") {
                REQUIRE_THAT(
                    scene_node_,
                    EqualsProto(ParseProto<SceneNode>(
                        "id: 'node_a' sprite_id: 'sprite_a' frame_index: 2")));

                AND_WHEN("progress is made and the animation repeats") {
                  REQUIRE_FALSE(performer.Progress(5, &scene_node_));

                  THEN("the frame-range animation rewinds") {
                    REQUIRE_THAT(scene_node_,
                                 EqualsProto(ParseProto<SceneNode>(
                                     "id: 'node_a' sprite_id: 'sprite_a' "
                                     "frame_index: 0")));

                    AND_WHEN("enough progress is made") {
                      REQUIRE(performer.Progress(10, &scene_node_));

                      THEN("the frame-range animation terminates") {
                        REQUIRE_THAT(scene_node_,
                                     EqualsProto(ParseProto<SceneNode>(
                                         "id: 'node_a' sprite_id: 'sprite_a' "
                                         "frame_index: 2")));
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    WHEN("the range is over a single frame ") {
      frame_range.set_start_frame(0);
      frame_range.set_end_frame(1);
      frame_range.set_repeat(0);

      FrameRangePerformer performer(frame_range, &core_);
      performer.Start(&scene_node_);

      THEN("it immediately applies its first frame") {
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

        AND_WHEN("more progress is made") {
          REQUIRE_FALSE(performer.Progress(5, &scene_node_));

          THEN("the same frame is applied again") {
            REQUIRE_THAT(
                scene_node_,
                EqualsProto(ParseProto<SceneNode>(
                    "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

            REQUIRE_FALSE(performer.Progress(5, &scene_node_));
            REQUIRE_THAT(
                scene_node_,
                EqualsProto(ParseProto<SceneNode>(
                    "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));
          }
        }
      }
    }

    WHEN("end frame is smaller than start frame") {
      frame_range = ParseProto<FrameRangeAnimation>(R"(
        start_frame: 3
        end_frame: 0
        delay: 5
        )");

      FrameRangePerformer performer(frame_range, &core_);
      performer.Start(&scene_node_);

      THEN("frames are applied in descending order") {
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 3")));

        REQUIRE_FALSE(performer.Progress(5, &scene_node_));
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 2")));

        REQUIRE_FALSE(performer.Progress(5, &scene_node_));
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 1")));

        REQUIRE_FALSE(performer.Progress(5, &scene_node_));
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 3")));
      }
    }
  }
}

SCENARIO_METHOD(PerformerFixture, "FrameList animation", "[performers]") {
  GIVEN("a frame-list animation") {
    auto frame_list = ParseProto<FrameListAnimation>(R"(
        frame: [0, 2, 4, 6]
        delay: 5
        repeat: 2
        )");

    WHEN("it starts") {
      FrameListPerformer performer(frame_list, &core_);
      performer.Start(&scene_node_);

      THEN("it immediately applies its first frame") {
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

        AND_WHEN("progress is made it applies frames till it terminates") {
          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 2")));

          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 4")));

          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 6")));

          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

          REQUIRE(performer.Progress(15, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 6")));
        }
      }
    }

    WHEN("the list has only one frame") {
      frame_list = ParseProto<FrameListAnimation>(R"(
          frame: [0]
          delay: 5
          )");
      FrameListPerformer performer(frame_list, &core_);
      performer.Start(&scene_node_);

      THEN("it immediately applies its first frame") {
        REQUIRE_THAT(scene_node_,
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

        AND_WHEN("more progress is made it applies the same frame") {
          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));

          REQUIRE_FALSE(performer.Progress(5, &scene_node_));
          REQUIRE_THAT(
              scene_node_,
              EqualsProto(ParseProto<SceneNode>(
                  "id: 'node_a' sprite_id: 'sprite_a' frame_index: 0")));
        }
      }
    }
  }
}

SCENARIO_METHOD(PerformerFixture, "Flash animation", "[performers]") {
  GIVEN("a flash animation") {
    auto flash = ParseProto<FlashAnimation>(R"(
        delay: 5
        repeat: 2
        )");

    FlashPerformer performer(flash);
    performer.Start(&scene_node_);

    WHEN("progress is made, the node becomes flashing (invisible/visible)") {
      REQUIRE_FALSE(performer.Progress(5, &scene_node_));
      REQUIRE_THAT(scene_node_,
                   EqualsProto(ParseProto<SceneNode>(
                       "id: 'node_a' sprite_id: 'sprite_a' visible: false")));

      REQUIRE(performer.Progress(5, &scene_node_));
      REQUIRE_THAT(scene_node_,
                   EqualsProto(ParseProto<SceneNode>(
                       "id: 'node_a' sprite_id: 'sprite_a' visible: true")));
    }
  }
}

SCENARIO_METHOD(PerformerFixture, "Goto animation", "[performers]") {
  GIVEN("a goto animation") {
    auto goto_animation = ParseProto<GotoAnimation>(R"(
        destination { x: 10  y: 5  z: 0 }
        step: 2
        delay: 1
        )");

    GotoPerformer performer(goto_animation);

    WHEN("progress is made") {
      performer.Start(&scene_node_);
      REQUIRE_FALSE(performer.Progress(2, &scene_node_));

      THEN("node moves to the right direction") {
        REQUIRE(scene_node_.position().x() == Approx(3.57).margin(0.01));
        REQUIRE(scene_node_.position().y() == Approx(1.79).margin(0.01));
        REQUIRE(scene_node_.position().z() == 0);
      }

      AND_WHEN("it reaches the destination") {
        REQUIRE(performer.Progress(10, &scene_node_));

        THEN("the node's position is the destination") {
          // Final destination is assigned to avoid precision errors.
          REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(
                                        "id: 'node_a' sprite_id: 'sprite_a' "
                                        "position { x: 10 y: 5 z: 0 }")));
        }
      }
    }

    WHEN("the node is already there") {
      *scene_node_.mutable_position() =
          ParseProto<Vector>("x : 10 y : 5 z : 0");
      performer.Start(&scene_node_);

      REQUIRE(performer.Progress(1, &scene_node_));
      REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(
                                    "id: 'node_a' sprite_id: 'sprite_a' "
                                    "position { x: 10 y: 5 z: 0 }")));
    }

    WHEN("node is moved by other source during goto") {
      THEN("goto has cached and applies a wrong direction") {
        // TODO(bourdenas): fix this bug.
      }
    }
  }
}

SCENARIO_METHOD(PerformerFixture, "Timer animation", "[performers]") {
  GIVEN("a timer animation") {
    const auto timer = ParseProto<TimerAnimation>("delay: 1000");

    TimerPerformer performer(timer);

    WHEN("it starts, it needs to pass time equal to delay to terminate") {
      performer.Start(&scene_node_);
      REQUIRE_FALSE(performer.Progress(500, &scene_node_));
      REQUIRE_FALSE(performer.Progress(200, &scene_node_));
      REQUIRE(performer.Progress(300, &scene_node_));
      REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(
                                    "id: 'node_a' sprite_id: 'sprite_a' ")));
    }
  }
}

}  // namespace troll
