#include "animation/script-animator.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "core/collision-checker.h"
#include "core/event-dispatcher.h"
#include "core/scene-manager.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-core.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class ScriptAnimatorFixture {
 public:
  ScriptAnimatorFixture() {
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film{} film{} film{} film{} film{} film{} film{})"));

    core_.set_resource_manager(&resource_manager_);
    core_.set_scene_manager(&scene_manager_);
    core_.set_collision_checker(&collision_checker_);
    core_.set_event_dispatcher(&event_dispatcher_);

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));
  }

 protected:
  TestCore core_;
  ResourceManager resource_manager_;
  SceneManager scene_manager_ =
      SceneManager(&resource_manager_, nullptr, &core_);
  CollisionChecker collision_checker_ =
      CollisionChecker(&scene_manager_, nullptr, &core_);
  EventDispatcher event_dispatcher_;

  TestingResourceManager testing_resource_manager_ =
      TestingResourceManager(&resource_manager_);
};

SCENARIO_METHOD(ScriptAnimatorFixture, "Script with single animation",
                "[script_animator]") {
  GIVEN("a single animation script") {
    auto script = ParseProto<AnimationScript>(R"(
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");

    WHEN("started for running indefinitely") {
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("script runs forever") {
        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(500);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 102  y: 0  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());
      }
    }

    WHEN("started for running a few times") {
      script.mutable_animation(0)->mutable_translation()->set_repeat(3);
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("script runs until it repeats enough times") {
        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(500);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 3  y: 0  z: 0 }")));

        REQUIRE_FALSE(script_animator.is_running());
        REQUIRE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("more progress is done on the finished animator") {
          script_animator.Progress(10);

          THEN("it has no effect") {
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 3  y: 0  z: 0 }")));

            REQUIRE_FALSE(script_animator.is_running());
            REQUIRE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(ScriptAnimatorFixture, "Script with multiple legs",
                "[script_animator]") {
  GIVEN("an animation script with multiple legs") {
    auto script = ParseProto<AnimationScript>(R"(
        animation {
          translation {
            vec { x: 1 }
            delay: 5
            repeat: 3
          }
          frame_list {
            frame: [2, 3]
            delay: 10
          }
        }
        animation {
          flash {
            delay: 2
            repeat: 3
          }
        })");

    WHEN("started") {
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("script is running") {
        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("the first leg finishes") {
          script_animator.Progress(15);

          THEN("script is still running") {
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "frame_index: 2 "
                             "position { x: 3  y: 0  z: 0 }")));

            REQUIRE(script_animator.is_running());
            REQUIRE_FALSE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());

            AND_WHEN("the second leg finishes") {
              script_animator.Progress(6);

              THEN("the script terminates") {
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_a' sprite_id: 'sprite_a' "
                                 "frame_index: 2 "
                                 "visible: false "
                                 "position { x: 3  y: 0  z: 0 }")));

                REQUIRE_FALSE(script_animator.is_running());
                REQUIRE(script_animator.is_finished());
                REQUIRE_FALSE(script_animator.is_paused());
              }
            }
          }
        }
      }
    }

    WHEN("started") {
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("script is running") {
        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("the first leg overruns") {
          script_animator.Progress(20);

          // TODO(bourdenas): Script legs only transition across different
          // Progress() calls. If for some reason rendering falls behind and has
          // to catch up, it might be possible that it can jitter because
          // remaining progress should be consumed by next leg, but instead is
          // thrown away.
          // NOTE: There is a similar bug on RepeatablePerformerBase::Progress()
          // that does the same for restarting an animation.
          THEN("script should transition to second leg, but it does not") {
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "frame_index: 2 "
                             "position { x: 3  y: 0  z: 0 }")));

            REQUIRE(script_animator.is_running());
            REQUIRE_FALSE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());

            AND_WHEN("the second leg finishes") {
              script_animator.Progress(6);

              THEN("the script terminates") {
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_a' sprite_id: 'sprite_a' "
                                 "frame_index: 2 "
                                 "visible: false "
                                 "position { x: 3  y: 0  z: 0 }")));

                REQUIRE_FALSE(script_animator.is_running());
                REQUIRE(script_animator.is_finished());
                REQUIRE_FALSE(script_animator.is_paused());
              }
            }
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(ScriptAnimatorFixture, "Script is repeated",
                "[script_animator]") {
  GIVEN("a script that is repeatable") {
    const auto script = ParseProto<AnimationScript>(R"(
        repeat: 2
        animation {
          translation {
            vec { x: 1 }
            delay: 5
            repeat: 3
          }
        }
        animation {
          translation {
            vec { y: 1 }
            delay: 5
            repeat: 3
          }
        })");

    WHEN("started") {
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("runs its two legs but does not finish") {
        script_animator.Progress(15);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 3  y: 0  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(15);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 3  y: 3  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("runs the two legs again") {
          THEN("it finishes") {
            script_animator.Progress(15);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 6  y: 3  z: 0 }")));

            REQUIRE(script_animator.is_running());
            REQUIRE_FALSE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());

            script_animator.Progress(15);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 6  y: 6  z: 0 }")));

            REQUIRE_FALSE(script_animator.is_running());
            REQUIRE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(ScriptAnimatorFixture, "Scripts on nodes that die",
                "[script_animator]") {
  GIVEN("an animation script") {
    const auto script = ParseProto<AnimationScript>(R"(
      animation {
        translation {
          vec { x: 1 }
          delay: 5
        }
      })");

    WHEN("started on a non-existing scene node") {
      ScriptAnimator script_animator(script, "non_existent_node", &core_);
      script_animator.Start();

      THEN("animation does not start") {
        REQUIRE_FALSE(script_animator.is_running());
        REQUIRE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("making progress on it") {
          script_animator.Progress(5);

          THEN(" has no effect") {
            REQUIRE(scene_manager_.GetSceneNodeById("non_existent_node") ==
                    nullptr);
            REQUIRE_FALSE(script_animator.is_running());
            REQUIRE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());
          }
        }
      }
    }

    WHEN("started") {
      ScriptAnimator script_animator(script, "node_a", &core_);
      script_animator.Start();

      THEN("animation runs") {
        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        script_animator.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        REQUIRE(script_animator.is_running());
        REQUIRE_FALSE(script_animator.is_finished());
        REQUIRE_FALSE(script_animator.is_paused());

        AND_WHEN("its scene node is deleted") {
          // Create a new scene manager as a hacky way to delete 'node_a'.
          scene_manager_ = SceneManager(&resource_manager_, nullptr, &core_);
          core_.set_scene_manager(&scene_manager_);

          THEN("the animator stops gracefully") {
            script_animator.Progress(10);
            REQUIRE(scene_manager_.GetSceneNodeById("node_a") == nullptr);
            REQUIRE_FALSE(script_animator.is_running());
            REQUIRE(script_animator.is_finished());
            REQUIRE_FALSE(script_animator.is_paused());
          }
        }
      }
    }
  }
}

}  // namespace troll
