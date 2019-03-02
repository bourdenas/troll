#include "animation/animator-manager.h"

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

class AnimatorManagerFixture {
 public:
  AnimatorManagerFixture() {
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film { width: 10  height: 10 }
        film { width: 20  height: 20 }
        film { width: 30  height: 30 })"));

    core_.set_resource_manager(&resource_manager_);
    core_.set_scene_manager(&scene_manager_);
    core_.set_collision_checker(&collision_checker_);
    core_.set_event_dispatcher(&event_dispatcher_);
  }

 protected:
  TestCore core_;
  ResourceManager resource_manager_;
  SceneManager scene_manager_ =
      SceneManager(&resource_manager_, nullptr, &core_);
  CollisionChecker collision_checker_ =
      CollisionChecker(&scene_manager_, nullptr, &core_);
  AnimatorManager animator_manager_ = AnimatorManager(&core_);
  EventDispatcher event_dispatcher_;

  TestingResourceManager testing_resource_manager_ =
      TestingResourceManager(&resource_manager_);
};

SCENARIO_METHOD(AnimatorManagerFixture, "Play node animation",
                "[animator_manager]") {
  GIVEN("an animation script and scene node") {
    const auto script = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");
    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    WHEN("animation starts") {
      animator_manager_.Play(script, "node_a");

      THEN("the animation is applied on node") {
        animator_manager_.Progress(12);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        // Animation will never stop executing by iteself.
        animator_manager_.Progress(5);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 3  y: 0  z: 0 }")));

        AND_WHEN("paused") {
          animator_manager_.Pause("script_a", "node_a");

          THEN("the animation is not applied") {
            animator_manager_.Progress(5);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 3  y: 0  z: 0 }")));

            AND_WHEN("resumed") {
              animator_manager_.Resume("script_a", "node_a");

              THEN("the animation is applied") {
                animator_manager_.Progress(5);
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_a' sprite_id: 'sprite_a' "
                                 "position { x: 4  y: 0  z: 0 }")));

                AND_WHEN("stopped") {
                  animator_manager_.Stop("script_a", "node_a");

                  THEN("the animation is not applied") {
                    animator_manager_.Progress(5);
                    REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                                 EqualsProto(ParseProto<SceneNode>(
                                     "id: 'node_a' sprite_id: 'sprite_a' "
                                     "position { x: 4  y: 0  z: 0 }")));

                    AND_WHEN("resumed") {
                      animator_manager_.Resume("script_a", "node_a");

                      THEN("the animation cannot be applied") {
                        animator_manager_.Progress(5);
                        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                                     EqualsProto(ParseProto<SceneNode>(
                                         "id: 'node_a' sprite_id: 'sprite_a' "
                                         "position { x: 4  y: 0  z: 0 }")));
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
  }
}

SCENARIO_METHOD(AnimatorManagerFixture, "Play animations",
                "[animator_manager]") {
  GIVEN("animation scripts and nodes") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");
    const auto script_b = ParseProto<AnimationScript>(R"(
        id: 'script_b'
        animation {
          translation {
            vec { y: 1 }
            delay: 10
          }
        })");

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));
    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_b' sprite_id: 'sprite_a'"));
    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_c' sprite_id: 'sprite_a'"));

    WHEN("animations start playing") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Play(script_b, "node_b");
      animator_manager_.Play(script_a, "node_c");

      THEN("all animations are applied") {
        animator_manager_.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_b' sprite_id: 'sprite_a' "
                         "position { x: 0  y: 1  z: 0 }")));

        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_c"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_c' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        AND_WHEN("an animation is paused") {
          animator_manager_.Pause("script_a", "node_c");

          THEN("the rest animation are still applied") {
            animator_manager_.Progress(10);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 4  y: 0  z: 0 }")));

            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_b' sprite_id: 'sprite_a' "
                             "position { x: 0  y: 2  z: 0 }")));

            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_c"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_c' sprite_id: 'sprite_a' "
                             "position { x: 2  y: 0  z: 0 }")));

            AND_WHEN("resumed") {
              animator_manager_.Resume("script_a", "node_c");

              THEN("the animation is applied") {
                animator_manager_.Progress(10);
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_a' sprite_id: 'sprite_a' "
                                 "position { x: 6  y: 0  z: 0 }")));

                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_b' sprite_id: 'sprite_a' "
                                 "position { x: 0  y: 3  z: 0 }")));

                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_c"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_c' sprite_id: 'sprite_a' "
                                 "position { x: 4  y: 0  z: 0 }")));

                AND_WHEN("a node's animations are stopped") {
                  animator_manager_.StopNodeAnimations("node_a");

                  THEN("the animation is applied") {
                    animator_manager_.Progress(10);
                    REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                                 EqualsProto(ParseProto<SceneNode>(
                                     "id: 'node_a' sprite_id: 'sprite_a' "
                                     "position { x: 6  y: 0  z: 0 }")));

                    REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                                 EqualsProto(ParseProto<SceneNode>(
                                     "id: 'node_b' sprite_id: 'sprite_a' "
                                     "position { x: 0  y: 4  z: 0 }")));

                    REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_c"),
                                 EqualsProto(ParseProto<SceneNode>(
                                     "id: 'node_c' sprite_id: 'sprite_a' "
                                     "position { x: 6  y: 0  z: 0 }")));
                  }
                }
              }
            }
          }
        }

        AND_WHEN("an animation is paused on a node that is not attached") {
          animator_manager_.Pause("script_a", "node_b");

          THEN("no animation is paused") {
            animator_manager_.Progress(10);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 4  y: 0  z: 0 }")));

            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_b' sprite_id: 'sprite_a' "
                             "position { x: 0  y: 2  z: 0 }")));

            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_c"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_c' sprite_id: 'sprite_a' "
                             "position { x: 4  y: 0  z: 0 }")));
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(AnimatorManagerFixture, "Play many scripts on a node",
                "[animator_manager]") {
  GIVEN("some scripts applied on a node") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");
    const auto script_b = ParseProto<AnimationScript>(R"(
        id: 'script_b'
        animation {
          translation {
            vec { y: 1 }
            delay: 10
          }
        })");
    const auto script_c = ParseProto<AnimationScript>(R"(
        id: 'script_c'
        animation {
          translation {
            vec { z: 1 }
            delay: 10
          }
        })");

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    WHEN("scripts start playing") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Play(script_b, "node_a");
      animator_manager_.Play(script_c, "node_a");

      THEN("all animations are applied") {
        animator_manager_.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 1  z: 1 }")));

        AND_WHEN("an animation is paused") {
          animator_manager_.Pause("script_a", "node_a");

          THEN("the rest animation are still applied") {
            animator_manager_.Progress(10);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(ParseProto<SceneNode>(
                             "id: 'node_a' sprite_id: 'sprite_a' "
                             "position { x: 2  y: 2  z: 2 }")));

            AND_WHEN("all node animation are stoped") {
              animator_manager_.StopNodeAnimations("node_a");

              THEN("no animation is applied") {
                animator_manager_.Progress(10);
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(ParseProto<SceneNode>(
                                 "id: 'node_a' sprite_id: 'sprite_a' "
                                 "position { x: 2  y: 2  z: 2 }")));
              }
            }
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(AnimatorManagerFixture, "Play many scripts on node that die",
                "[animator_manager]") {
  GIVEN("a script") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");

    WHEN("applied on a non-existing node") {
      animator_manager_.Play(script_a, "node_a");

      THEN("nothing happens") {
        animator_manager_.Progress(10);
        REQUIRE(scene_manager_.GetSceneNodeById("node_a") == nullptr);
      }
    }

    WHEN("applied on a node") {
      scene_manager_.AddSceneNode(
          ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));
      animator_manager_.Play(script_a, "node_a");

      THEN("it starts") {
        animator_manager_.Progress(10);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(ParseProto<SceneNode>(
                         "id: 'node_a' sprite_id: 'sprite_a' "
                         "position { x: 2  y: 0  z: 0 }")));

        AND_WHEN("the node dies") {
          // Create a new scene manager as a hacky way to delete 'node_a'.
          scene_manager_ = SceneManager(&resource_manager_, nullptr, &core_);
          core_.set_scene_manager(&scene_manager_);

          THEN("the animation stops without crashes") {
            animator_manager_.Progress(10);
            REQUIRE(scene_manager_.GetSceneNodeById("node_a") == nullptr);
          }
        }
      }
    }
  }
}

}  // namespace troll
