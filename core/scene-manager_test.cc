#include "core/scene-manager.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "animation/animator-manager.h"
#include "core/troll-core.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class SceneManagerFixture {
 public:
  SceneManagerFixture() {
    scene_manager_ = new SceneManager();
    Core::Instance().SetupTestSceneManager(scene_manager_);

    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film { width: 10  height: 10 }
        film { width: 20  height: 20 }
        film { width: 30  height: 30 })"));
  }

  virtual ~SceneManagerFixture() {
    AnimatorManager::Instance().StopAll();
    ResourceManager::Instance().CleanUp();
  }

 protected:
  SceneManager* scene_manager_ = nullptr;
};

SCENARIO_METHOD(SceneManagerFixture, "Running animation scripts on scene nodes",
                "[SceneManager.RunScript") {
  GIVEN("An animation script that is repeatable indefinitely") {
    TestingResourceManager::SetTestAnimationScript(
        ParseProto<AnimationScript>(R"(
            id: 'script_a'
            animation {
              translation {
                vec { x: 1 }
                delay: 5
              }
            })"));

    scene_manager_->AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
          id: 'node_a' sprite_id: 'sprite_a'
          position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(12);

      THEN("the scene node is affected") {
        expected.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("more progress is made") {
          AnimatorManager::Instance().Progress(20);

          THEN("animation keeps being applied") {
            expected.mutable_position()->set_x(6);
            REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                         EqualsProto(expected));

            AND_WHEN("the animation is stopped") {
              AnimatorManager::Instance().Stop("script_a", "node_a");
              AnimatorManager::Instance().Progress(5);

              THEN("the animation is no longer applied") {
                REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                             EqualsProto(expected));
              }
            }

            AND_WHEN("the animation is paused") {
              AnimatorManager::Instance().Pause("script_a", "node_a");
              AnimatorManager::Instance().Progress(5);

              THEN("the animation is not applied") {
                REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                             EqualsProto(expected));

                AND_WHEN("the animation is resumed") {
                  AnimatorManager::Instance().Resume("script_a", "node_a");
                  AnimatorManager::Instance().Progress(15);

                  THEN("the animation is applied again") {
                    expected.mutable_position()->set_x(9);
                    REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
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
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(12);

      THEN("the animation effect is doubles") {
        expected.mutable_position()->set_x(4);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected));
      }
    }
  }
}

SCENARIO_METHOD(SceneManagerFixture, "Running finite scripts on scene nodes",
                "[SceneManager.RunFiniteScript") {
  GIVEN("An animation script that is repeated only twice") {
    TestingResourceManager::SetTestAnimationScript(
        ParseProto<AnimationScript>(R"(
            id: 'script_a'
            animation {
              translation {
                vec { x: 1 }
                delay: 5
                repeat: 2
              }
            })"));

    scene_manager_->AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node till end") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(20);

      THEN("the animation is only applied twice") {
        expected.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected));
      }
    }

    WHEN("it starts running on a scene node") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(5);

      THEN("the animation is applied") {
        expected.mutable_position()->set_x(1);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("the animation is stopped") {
          AnimatorManager::Instance().Stop("script_a", "node_a");
          AnimatorManager::Instance().Progress(5);

          THEN("the animation is no longer applied") {
            REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                         EqualsProto(expected));
          }
        }

        AND_WHEN("the animation is paused") {
          AnimatorManager::Instance().Pause("script_a", "node_a");
          AnimatorManager::Instance().Progress(5);

          THEN("the animation is not applied") {
            REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                         EqualsProto(expected));

            AND_WHEN("the animation is resumed") {
              AnimatorManager::Instance().Resume("script_a", "node_a");
              AnimatorManager::Instance().Progress(15);

              THEN("the animation is applied again") {
                expected.mutable_position()->set_x(2);
                REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
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
    TestingResourceManager::SetTestAnimationScript(
        ParseProto<AnimationScript>(R"(
            id: 'script_a'
            animation {
              translation {
                vec { x: 1 }
                delay: 5
              }
            })"));

    WHEN("applied on a non-existing scene node") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(5);

      THEN("it is a noop") {
        REQUIRE(scene_manager_->GetSceneNodeById("node_a") == nullptr);
      }
    }

    WHEN("stopped on a non-existing scene node") {
      AnimatorManager::Instance().Stop("script_a", "node_a");
      AnimatorManager::Instance().Progress(5);

      THEN("it is a noop") {
        REQUIRE(scene_manager_->GetSceneNodeById("node_a") == nullptr);
      }
    }

    scene_manager_->AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));

    auto expected = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("applied on a scene node") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Progress(5);

      THEN("it makes some progress") {
        expected.mutable_position()->set_x(1);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected));

        AND_WHEN("the scene node is destroyed but the script keeps running") {
          // Create a new scene manager as a hacky way to destory the node.
          scene_manager_ = new SceneManager();
          Core::Instance().SetupTestSceneManager(scene_manager_);

          AnimatorManager::Instance().Progress(50);

          THEN("expect no death") { SUCCEED(); }
        }
      }
    }
  }
}

SCENARIO_METHOD(SceneManagerFixture, "Scene nodes with multiple scripts",
                "[SceneManager.MultiScriptNodes") {
  GIVEN("Some animations scripts applied on some scene nodes") {
    TestingResourceManager::SetTestAnimationScript(
        ParseProto<AnimationScript>(R"(
            id: 'script_a'
            animation {
              translation {
                vec { x: 1 }
                delay: 5
              }
            })"));
    TestingResourceManager::SetTestAnimationScript(
        ParseProto<AnimationScript>(R"(
            id: 'script_b'
            animation {
              translation {
                vec { y: 1 }
                delay: 5
              }
            })"));

    scene_manager_->AddSceneNode(
        ParseProto<SceneNode>("id: 'node_a' sprite_id: 'sprite_a'"));
    scene_manager_->AddSceneNode(
        ParseProto<SceneNode>("id: 'node_b' sprite_id: 'sprite_a'"));

    auto expected_a = ParseProto<SceneNode>(R"(
        id: 'node_a' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");
    auto expected_b = ParseProto<SceneNode>(R"(
        id: 'node_b' sprite_id: 'sprite_a'
        position { x: 0  y: 0  z: 0 })");

    WHEN("multiple scripts are applied on a node") {
      AnimatorManager::Instance().Play("script_a", "node_a");
      AnimatorManager::Instance().Play("script_b", "node_a");
      AnimatorManager::Instance().Play("script_a", "node_b");
      AnimatorManager::Instance().Progress(10);

      THEN("all of them are applied") {
        expected_a.mutable_position()->set_x(2);
        expected_a.mutable_position()->set_y(2);
        expected_b.mutable_position()->set_x(2);
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                     EqualsProto(expected_a));
        REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_b"),
                     EqualsProto(expected_b));

        AND_WHEN("all animations for a node are stopped") {
          AnimatorManager::Instance().StopNodeAnimations("node_a");
          AnimatorManager::Instance().Progress(5);

          THEN("it has no effect on other nodes") {
            expected_b.mutable_position()->set_x(3);
            REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_a"),
                         EqualsProto(expected_a));
            REQUIRE_THAT(*scene_manager_->GetSceneNodeById("node_b"),
                         EqualsProto(expected_b));
          }
        }
      }
    }
  }
}

}  // namespace troll
