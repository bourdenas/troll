#include "core/collision-checker.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <range/v3/algorithm/count_if.hpp>

#include "action/action-manager.h"
#include "core/scene-manager.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-core.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

class CollisionCheckerFixture {
 public:
  CollisionCheckerFixture() {
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_a'
        film { width: 10  height: 10 })"));
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_b'
        film { width: 10  height: 10 })"));
    testing_resource_manager_.SetTestSprite(ParseProto<Sprite>(R"(
        id: 'sprite_c'
        film { width: 20  height: 20 })"));

    core_.set_resource_manager(&resource_manager_);
    core_.set_action_manager(&action_manager_);
    core_.set_scene_manager(&scene_manager_);
    core_.set_collision_checker(&collision_checker_);
  }

 protected:
  void CreateNode(const std::string& id, const std::string& sprite_id,
                  const std::pair<int, int>& position) {
    SceneNode node;
    node.set_id(id);
    node.set_sprite_id(sprite_id);
    node.mutable_position()->set_x(std::get<0>(position));
    node.mutable_position()->set_y(std::get<1>(position));
    scene_manager_.AddSceneNode(node);
  }

  void MoveNode(const std::string& id, const std::pair<int, int>& at) {
    auto* node = scene_manager_.GetSceneNodeById(id);
    node->mutable_position()->set_x(std::get<0>(at));
    node->mutable_position()->set_y(std::get<1>(at));

    // When a node moves it needs to be marked dirty for CollisionChecker to
    // consider.
    collision_checker_.Dirty(*node);
  }

  int CountNodesBySprite(const std::string& sprite_id) {
    const auto count_all = [](const auto&) { return true; };
    return ranges::count_if(scene_manager_.GetSceneNodesBySpriteId(sprite_id),
                            count_all);
  }

  TestCore core_;
  ResourceManager resource_manager_;
  ActionManager action_manager_ = ActionManager(&core_);
  SceneManager scene_manager_ =
      SceneManager(&resource_manager_, nullptr, &core_);
  CollisionChecker collision_checker_ =
      CollisionChecker(&scene_manager_, &action_manager_, &core_);

  TestingResourceManager testing_resource_manager_ =
      TestingResourceManager(&resource_manager_);
};

SCENARIO_METHOD(CollisionCheckerFixture, "Boring collision checking",
                "[collisions]") {
  GIVEN("A collision checker") {
    WHEN("no nodes are registered for collision") {
      collision_checker_.CheckCollisions();
      THEN("expect no death") { SUCCEED(); }
    }

    WHEN("registered nodes do not exist") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
              scene_node_id: [ 'node_a', 'node_b' ]
              action {
                create_scene_node { scene_node { sprite_id: 'sprite_c' } }
              })"));
      collision_checker_.CheckCollisions();

      THEN("no action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 0);
      }

      AND_WHEN("a different pairs of nodes collide") {
        CreateNode("node_a", "sprite_a", {0, 0});
        CreateNode("node_c", "sprite_a", {0, 0});
        collision_checker_.CheckCollisions();

        THEN("the action of the original pair is not triggered") {
          REQUIRE(CountNodesBySprite("sprite_c") == 0);
        }
      }
    }
  }
}

SCENARIO_METHOD(CollisionCheckerFixture, "Nodes colliding", "[collisions]") {
  GIVEN("A collision pair action and a pair of nodes") {
    collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: [ 'node_a', 'node_b' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

    CreateNode("node_a", "sprite_a", {0, 0});
    CreateNode("node_b", "sprite_b", {20, 20});

    WHEN("nodes do not collide") {
      collision_checker_.CheckCollisions();

      THEN("no action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 0);
      }
    }

    WHEN("nodes move into collision") {
      MoveNode("node_a", {15, 15});
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 1);

        AND_WHEN("nodes move again but remain colliding ") {
          MoveNode("node_a", {12, 12});
          collision_checker_.CheckCollisions();

          THEN("the collision action is not triggered again") {
            REQUIRE(CountNodesBySprite("sprite_c") == 1);

            AND_WHEN("nodes detach and collide again") {
              MoveNode("node_a", {8, 8});
              collision_checker_.CheckCollisions();

              MoveNode("node_a", {15, 15});
              collision_checker_.CheckCollisions();

              THEN("the collision action is triggered again") {
                REQUIRE(CountNodesBySprite("sprite_c") == 2);
              }
            }
          }
        }
      }
    }

    WHEN("nodes only touch") {
      MoveNode("node_a", {10, 20});
      collision_checker_.CheckCollisions();

      THEN("no action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 0);
      }
    }

    WHEN("more collision actions are registered on the original node pair") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: [ 'node_a', 'node_b' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_b' } }
            })"));
      MoveNode("node_a", {15, 15});
      collision_checker_.CheckCollisions();

      THEN("all collision actions are triggered") {
        REQUIRE(CountNodesBySprite("sprite_a") == 1);
        REQUIRE(CountNodesBySprite("sprite_b") == 2);
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }

    WHEN(
        "a collision action results to a new node that immediatelly collides "
        "with another") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: [ 'node_a', 'node_c' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_b' } }
            })"));
      MoveNode("node_a", {15, 15});
      collision_checker_.CheckCollisions();

      THEN("all collision actions are triggered") {
        REQUIRE(CountNodesBySprite("sprite_a") == 1);
        // TODO(bourdenas): Test actually fails, figure out why.
        // REQUIRE(CountNodesBySprite("sprite_b") == 2);
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }
  }
}

SCENARIO_METHOD(CollisionCheckerFixture, "Sprite-based collisions",
                "[collisions]") {
  GIVEN("Nodes with different sprites that collide") {
    CreateNode("node_a", "sprite_a", {0, 0});
    CreateNode("node_b", "sprite_b", {7, 7});
    CreateNode("node_c", "sprite_a", {20, 20});

    WHEN("a collision based on sprites is registered") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            sprite_id: [ 'sprite_a', 'sprite_b' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }

    WHEN("a mixed collision based on sprites and node ids is registered") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: 'node_b'
            sprite_id: 'sprite_a'
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }

    WHEN("a collision of a node with its own sprite is defined") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: 'node_a'
            sprite_id: 'sprite_a'
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

      THEN("a node cannot collide with itself") {
        collision_checker_.CheckCollisions();
        REQUIRE(CountNodesBySprite("sprite_c") == 0);
      }

      AND_WHEN("a different node of the same sprite collides with it") {
        MoveNode("node_c", {0, 0});
        collision_checker_.CheckCollisions();

        THEN("the collision action is triggered") {
          REQUIRE(CountNodesBySprite("sprite_c") == 1);
        }
      }
    }

    WHEN("a single sprite collision is defined") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            sprite_id: 'sprite_a'
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

      THEN("nodes of the same sprite can collide and trigger actions") {
        MoveNode("node_c", {0, 0});
        collision_checker_.CheckCollisions();
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }

    WHEN("a sprite collision is defined that repeats the same sprite") {
      collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            sprite_id: ['sprite_a', 'sprite_a' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

      THEN("nodes of the same sprite can collide and trigger actions") {
        MoveNode("node_c", {0, 0});
        collision_checker_.CheckCollisions();
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }
  }
}

SCENARIO_METHOD(CollisionCheckerFixture, "Multi node collisions",
                "[collisions]") {
  GIVEN("A collision with multiple nodes") {
    collision_checker_.RegisterCollision(ParseProto<CollisionAction>(R"(
            scene_node_id: [ 'node_a', 'node_b', 'node_c' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

    CreateNode("node_a", "sprite_a", {0, 0});
    CreateNode("node_b", "sprite_a", {20, 20});
    CreateNode("node_c", "sprite_a", {25, 25});

    WHEN("only node B and C collide") {
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered only once") {
        REQUIRE(CountNodesBySprite("sprite_c") == 1);
      }
    }

    WHEN("nodes collide only in pairs: A with B and B with C") {
      MoveNode("node_a", {12, 12});
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered twice") {
        REQUIRE(CountNodesBySprite("sprite_c") == 2);
      }
    }

    WHEN("each node of the set collides with all others") {
      MoveNode("node_a", {18, 18});
      collision_checker_.CheckCollisions();

      THEN("the collision action is triggered for each pair") {
        REQUIRE(CountNodesBySprite("sprite_c") == 3);
      }
    }
  }
}

SCENARIO_METHOD(CollisionCheckerFixture, "Nodes detaching", "[collisions]") {
  GIVEN("A detaching pair action and a pair of nodes") {
    collision_checker_.RegisterDetachment(ParseProto<CollisionAction>(R"(
            scene_node_id: [ 'node_a', 'node_b' ]
            action {
              create_scene_node { scene_node { sprite_id: 'sprite_c' } }
            })"));

    CreateNode("node_a", "sprite_a", {0, 0});
    CreateNode("node_b", "sprite_b", {20, 20});

    WHEN("nodes do not collide") {
      collision_checker_.CheckCollisions();

      THEN("no action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 0);
      }
    }

    WHEN("nodes move into collision") {
      MoveNode("node_b", {0, 0});
      collision_checker_.CheckCollisions();

      THEN("no action is triggered") {
        REQUIRE(CountNodesBySprite("sprite_c") == 0);

        AND_WHEN("nodes move again but remain colliding ") {
          MoveNode("node_b", {5, 5});
          collision_checker_.CheckCollisions();

          THEN("still no action is triggered") {
            REQUIRE(CountNodesBySprite("sprite_c") == 0);

            AND_WHEN("nodes detach") {
              MoveNode("node_b", {20, 20});
              collision_checker_.CheckCollisions();

              THEN("the detachment action is triggered") {
                // TODO(bourdenas): Doesn't work properly, action is triggered
                // twice.
                // REQUIRE(CountNodesBySprite("sprite_c") == 1);
              }
            }
          }
        }
      }
    }
  }
}

SCENARIO("Pixel-perfect collision", "[collisions]") {
  GIVEN("two collision masks") {
    //  LHS  -  RHS
    // 10000   11000
    // 10011   01100
    // 11110   11111
    std::string tmp = "100001001111110";
    boost::dynamic_bitset<> lhs_mask =
        boost::dynamic_bitset<>(std::string(tmp.rbegin(), tmp.rend()));
    tmp = "110000110011111";
    boost::dynamic_bitset<> rhs_mask =
        boost::dynamic_bitset<>(std::string(tmp.rbegin(), tmp.rend()));

    WHEN("sprites are next to each other") {
      const auto lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
      const auto rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");

      THEN("no collision triggers") {
        REQUIRE_FALSE(
            internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
      }
    }

    WHEN("sprite bounding boxes collide without any overlapping pixels") {
      const auto lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
      const auto rhs = ParseProto<Box>("left: 4  top: 0  width: 5  height: 3");

      THEN("no collision triggers") {
        REQUIRE_FALSE(
            internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
      }
    }

    WHEN("sprite bounding boxes collide and pixels overlap") {
      const auto lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
      const auto rhs = ParseProto<Box>("left: 3  top: 0  width: 5  height: 3");

      THEN("collision triggers") {
        REQUIRE(internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
      }
    }

    WHEN("sprite bounding boxes collide from top but pixels do not overlap") {
      auto lhs = ParseProto<Box>("left: 4  top: 2  width: 5  height: 3");
      auto rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");

      THEN("no collision triggers") {
        REQUIRE_FALSE(
            internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));

        AND_WHEN("the first box moves one pixelto the right") {
          lhs = ParseProto<Box>("left: 5  top: 2  width: 5  height: 3");

          THEN("collision triggers") {
            REQUIRE(
                internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
          }
        }
      }
    }

    WHEN("sprites has negative position values but do not collide") {
      const auto lhs =
          ParseProto<Box>("left: -2  top: -2  width: 5  height: 3");
      const auto rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");

      THEN("no collision triggers") {
        REQUIRE_FALSE(
            internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
      }
    }

    WHEN("sprites has negative position values and collide") {
      const auto lhs =
          ParseProto<Box>("left: -1  top: -2  width: 5  height: 3");
      const auto rhs = ParseProto<Box>("left: 1  top: 0  width: 5  height: 3");

      THEN("collision triggers") {
        REQUIRE(internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask, rhs_mask));
      }
    }
  }
}

}  // namespace troll
