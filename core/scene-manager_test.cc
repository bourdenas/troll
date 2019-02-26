#include "core/scene-manager.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/event-dispatcher.h"
#include "core/resource-manager.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-core.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class SceneManagerFixture {
 public:
  SceneManagerFixture() {
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film { width: 10  height: 10 }
        film { width: 20  height: 20 }
        film { width: 30  height: 30 })"));

    core_.set_resource_manager(&resource_manager_);
    core_.set_animator_manager(&animator_manager_);
    core_.set_scene_manager(&scene_manager_);
    core_.set_collision_checker(&collision_checker_);
    core_.set_event_dispatcher(&event_dispatcher_);
  }

 protected:
  TestCore core_;
  ResourceManager resource_manager_;
  AnimatorManager animator_manager_ = AnimatorManager(&core_);
  SceneManager scene_manager_ =
      SceneManager(&resource_manager_, nullptr, &core_);
  CollisionChecker collision_checker_ =
      CollisionChecker(&scene_manager_, nullptr, &core_);
  EventDispatcher event_dispatcher_;

  TestingResourceManager testing_resource_manager_ =
      TestingResourceManager(&resource_manager_);
};

SCENARIO_METHOD(SceneManagerFixture, "Running animation scripts on scene nodes",
                "[SceneManager.RunScript") {
  GIVEN("An animation script that is repeatable indefinitely") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
          id: 'node_a' sprite_id: 'sprite_a'
          position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(12);

      THEN("the scene node is affected") {
        expected.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("more progress is made") {
          animator_manager_.Progress(20);

          THEN("animation keeps being applied") {
            expected.mutable_position()->set_x(6);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(expected));

            AND_WHEN("the animation is stopped") {
              animator_manager_.Stop("script_a", "node_a");
              animator_manager_.Progress(5);

              THEN("the animation is no longer applied") {
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(expected));
              }
            }

            AND_WHEN("the animation is paused") {
              animator_manager_.Pause("script_a", "node_a");
              animator_manager_.Progress(5);

              THEN("the animation is not applied") {
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(expected));

                AND_WHEN("the animation is resumed") {
                  animator_manager_.Resume("script_a", "node_a");
                  animator_manager_.Progress(15);

                  THEN("the animation is applied again") {
                    expected.mutable_position()->set_x(9);
                    REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                                 EqualsProto(expected));
                  }
                }
              }
            }
          }
        }
      }
    }

    WHEN("applied on a scene node twice") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(12);

      THEN("the animation effect is doubles") {
        expected.mutable_position()->set_x(4);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected));
      }
    }
  }
}

SCENARIO_METHOD(SceneManagerFixture, "Running finite scripts on scene nodes",
                "[SceneManager.RunFiniteScript") {
  GIVEN("An animation script that is repeated only twice") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
            repeat: 2
          }
        })");

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node till end") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(20);

      THEN("the animation is only applied twice") {
        expected.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected));
      }
    }

    WHEN("it starts running on a scene node") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(5);

      THEN("the animation is applied") {
        expected.mutable_position()->set_x(1);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("the animation is stopped") {
          animator_manager_.Stop("script_a", "node_a");
          animator_manager_.Progress(5);

          THEN("the animation is no longer applied") {
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(expected));
          }
        }

        AND_WHEN("the animation is paused") {
          animator_manager_.Pause("script_a", "node_a");
          animator_manager_.Progress(5);

          THEN("the animation is not applied") {
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(expected));

            AND_WHEN("the animation is resumed") {
              animator_manager_.Resume("script_a", "node_a");
              animator_manager_.Progress(15);

              THEN("the animation is applied again") {
                expected.mutable_position()->set_x(2);
                REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                             EqualsProto(expected));
              }
            }
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(SceneManagerFixture, "Scripts on non-existing scene nodes",
                "[SceneManager.ScriptOnAbsentNodes") {
  GIVEN("An animation script") {
    const auto script_a = ParseProto<AnimationScript>(R"(
        id: 'script_a'
        animation {
          translation {
            vec { x: 1 }
            delay: 5
          }
        })");

    WHEN("applied on a non-existing scene node") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(5);

      THEN("it is a noop") {
        REQUIRE(scene_manager_.GetSceneNodeById("node_a") == nullptr);
      }
    }

    WHEN("stopped on a non-existing scene node") {
      animator_manager_.Stop("script_a", "node_a");
      animator_manager_.Progress(5);

      THEN("it is a noop") {
        REQUIRE(scene_manager_.GetSceneNodeById("node_a") == nullptr);
      }
    }

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Progress(5);

      THEN("it makes some progress") {
        expected.mutable_position()->set_x(1);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("the scene node is destroyed but the script keeps running") {
          // Create a new scene manager as a hacky way to destory the node.
          scene_manager_ = SceneManager(&resource_manager_, nullptr, &core_);
          animator_manager_.Progress(50);

          THEN("expect no death") { SUCCEED(); }
        }
      }
    }
  }
}

SCENARIO_METHOD(SceneManagerFixture, "Scene nodes with multiple scripts",
                "[SceneManager.MultiScriptNodes") {
  GIVEN("Some animations scripts applied on some scene nodes") {
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
            delay: 5
          }
        })");

    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));
    scene_manager_.AddSceneNode(
        ParseProto<SceneNode>("id: 'node_b' sprite_id: 'sprite_a'"));

    auto expected_a = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");
    auto expected_b = ParseProto<SceneNode>(R"(
        id: 'node_b' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("multiple scripts are applied on a node") {
      animator_manager_.Play(script_a, "node_a");
      animator_manager_.Play(script_b, "node_a");
      animator_manager_.Play(script_a, "node_b");
      animator_manager_.Progress(10);

      THEN("all of them are applied") {
        expected_a.mutable_position()->set_x(2);
        expected_a.mutable_position()->set_y(2);
        expected_b.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                     EqualsProto(expected_a));
        REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                     EqualsProto(expected_b));

        AND_WHEN("all animations for a node are stopped") {
          animator_manager_.StopNodeAnimations("node_a");
          animator_manager_.Progress(5);

          THEN("it has no effect on other nodes") {
            expected_b.mutable_position()->set_x(3);
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_a"),
                         EqualsProto(expected_a));
            REQUIRE_THAT(*scene_manager_.GetSceneNodeById("node_b"),
                         EqualsProto(expected_b));
          }
        }
      }
    }
  }
}

}  // namespace troll
