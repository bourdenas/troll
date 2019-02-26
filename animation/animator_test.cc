#include "animation/animator.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "core/scene-manager.h"
#include "proto/animation.pb.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-core.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class AnimatorFixture {
 public:
  AnimatorFixture() {
    testing_resource_manager_.SetTestSprite(
        ParseProto<Sprite>("id: 'sprite_a' film{} film{} film{}"));

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

SCENARIO_METHOD(AnimatorFixture,
                "Composite animation terminates when any part terminates",
                "[animator]") {
  GIVEN("a composite animation that is conditioned on ANY") {
    const auto composite_animation = ParseProto<Animation>(R"(
        termination: ANY
        translation {
          vec { x: 1 }
          delay: 10
        }
        frame_list {
          frame: [ 0, 1, 2 ]
          delay: 10
          repeat: 2
        })");

    Animator animator;
    animator.Start(composite_animation, &scene_node_, &core_);

    WHEN("some progress is done") {
      REQUIRE_FALSE(animator.Progress(10, &scene_node_));

      THEN("the animation is applied") {
        REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
            id: 'node_a' sprite_id: 'sprite_a'
            frame_index: 1
            position { x: 1 y: 0 z: 0 })")));

        AND_WHEN("one of the parts is finished") {
          REQUIRE(animator.Progress(40, &scene_node_));

          THEN("the animator is finished") {
            REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a' sprite_id: 'sprite_a'
                frame_index: 2
                position { x: 5 y: 0 z: 0 })")));
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(AnimatorFixture,
                "Composite animation terminates when all parts terminates",
                "[animator]") {
  GIVEN("a composite animation that is conditioned on ALL") {
    const auto composite_animation = ParseProto<Animation>(R"(
        termination: ALL
        translation {
          vec { x: 1 }
          delay: 10
          repeat: 1
        }
        frame_list {
          frame: [ 0, 1, 2 ]
          delay: 10
          repeat: 2
        })");

    Animator animator;
    animator.Start(composite_animation, &scene_node_, &core_);

    WHEN("the first animation is finished") {
      REQUIRE_FALSE(animator.Progress(10, &scene_node_));

      THEN("the animator is not done") {
        REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
            id: 'node_a' sprite_id: 'sprite_a'
            frame_index: 1
            position { x: 1 y: 0 z: 0 })")));

        AND_WHEN("all parts are finished") {
          REQUIRE(animator.Progress(40, &scene_node_));

          THEN("the animator is finished") {
            REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
                id: 'node_a' sprite_id: 'sprite_a'
                frame_index: 2
                position { x: 1 y: 0 z: 0 })")));
          }
        }
      }
    }
  }
}

SCENARIO_METHOD(
    AnimatorFixture,
    "Composite animation terminates when a non-repeatable part finishes",
    "[animator]") {
  GIVEN("a composite animation that is conditioned on ALL") {
    const auto composite_animation = ParseProto<Animation>(R"(
        termination: ANY
        translation {
          vec { x: 1 }
          delay: 10
        }
        frame_list {
          frame: [ 0, 1, 2 ]
          delay: 10
          repeat: 2
        }
        timer {
         delay: 30
        })");

    Animator animator;
    animator.Start(composite_animation, &scene_node_, &core_);

    WHEN("the non-repeatable animation is finished (timer)") {
      REQUIRE(animator.Progress(30, &scene_node_));

      THEN("the animator is done") {
        REQUIRE_THAT(scene_node_, EqualsProto(ParseProto<SceneNode>(R"(
            id: 'node_a' sprite_id: 'sprite_a'
            frame_index: 0
            position { x: 3 y: 0 z: 0 })")));
      }
    }
  }
}

}  // namespace troll
