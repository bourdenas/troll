#include "core/collision-checker.h"

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <range/v3/algorithm/count_if.hpp>

#include "action/action-manager.h"
#include "core/troll-core.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

using testing::Pointee;

class CollisionCheckerTest : public testing::Test {
 protected:
  void SetUp() override {
    scene_manager_ = new SceneManager();
    Core::Instance().SetupTestSceneManager(scene_manager_);

    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: 'sprite_a'
      film {
        width: 10  height: 10
      })"));
    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: 'sprite_b'
      film {
        width: 10  height: 10
      })"));
    TestingResourceManager::SetTestSprite(ParseProto<Sprite>(R"(
      id: 'sprite_c'
      film {
        width: 3  height: 3
      })"));

    CollisionChecker::Instance().Init();

    // The tests below use Actions as a proxy to very that collisions occur. The
    // effect of a collision is calling some action.
    ActionManager::Instance().Init();
  }

  void TearDown() override { ResourceManager::Instance().CleanUp(); }

  SceneManager* scene_manager_ = nullptr;
};

TEST_F(CollisionCheckerTest, CollisionCheckerIsBoringWithoutNodesCollisions) {
  CollisionChecker::Instance().CheckCollisions();
}

TEST_F(CollisionCheckerTest, CollisionCheckerStillBoringWithoutNodes) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node {
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

TEST_F(CollisionCheckerTest,
       CollisionCheckerStillBoringWithoutCollisionActions) {
  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 0  y: 0 })"));

  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

TEST_F(CollisionCheckerTest, NodesDontCollideNoActionExecuted) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 20  y: 20 })"));

  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

TEST_F(CollisionCheckerTest, NodesOverlapExecutingTheirAction) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  // CreateSceneNode action executes due to collision and creates new node.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    )"))));
}

TEST_F(CollisionCheckerTest, NodesTouchingDontTriggerCollision) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 10  y: 0 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Nodes A and B only touch theirs bounding box edges. Touching is not a
  // collision.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

TEST_F(CollisionCheckerTest, MultiNodeCollisionsArePairwise) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b', 'node_c' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 20  y: 20 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 25  y: 25 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Only node_b and node_c collide (C is inside B). The action executes once.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    )"))));
}

TEST_F(CollisionCheckerTest, MultiNodeCollisionsExecutePerPair) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b', 'node_c' ]
    action {
      create_scene_node {
        scene_node { 
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 3  y: 3 })"));

  auto count_all = [](const auto&) { return true; };
  EXPECT_EQ(
      1, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));

  CollisionChecker::Instance().CheckCollisions();

  // All three nodes collide together. A overlaps with B, B overlaps with C and
  // C is also fully inside A. The action will be executed 3 times, once for
  // each pair, resulting to 3 new nodes of sprite_c.
  EXPECT_EQ(
      4, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));
}

TEST_F(CollisionCheckerTest, SpriteBasedCollisions) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_b', 'sprite_c' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
          position { x: 80  y: 80 }
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 20  y: 20 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 25  y: 25 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Only node_b and node_c collide (C is inside B). The action executes once.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    position { 
                                      x: 80  y: 80
                                    })"))));
}

TEST_F(CollisionCheckerTest, NewlyCreatedNodesAlsoTriggerCollisions) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
          position { x: 0  y: 0 }
        }
      }
    })"));
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_c' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node_b'
          sprite_id: 'sprite_c'
          position { x: 80  y: 80 }
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Initially, only node_a and node_b collide creating created_node. The new
  // node subsequently collides with node_a causing creation of created_node_b.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    position { 
                                      x: 0  y: 0
                                    })"))));
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node_b"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node_b'
                                    sprite_id: 'sprite_c'
                                    position { 
                                      x: 80  y: 80
                                    })"))));
}

TEST_F(CollisionCheckerTest, NodesCollideWithoutMatchingSprites) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Even though nodes collide they don't have a registered action for their
  // collision.
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

TEST_F(CollisionCheckerTest, NodeAndSpritesCanBeCombinedForCollisions) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a' ]
    sprite_id: [ 'sprite_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Here node_a can collide with nodes of sprite_b, i.e. node_b.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c')"))));
}

TEST_F(CollisionCheckerTest,
       CollidingNodesDontTriggerActionsAgainUnlessDetachFirst) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  auto count_all = [](const auto&) { return true; };
  EXPECT_EQ(
      0, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));

  // First collision creates a new scene_node.
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(
      1, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));

  // Break constness rules locally, to allow modify node's position for testing.
  SceneNode* node_a =
      const_cast<SceneNode*>(scene_manager_->GetSceneNodeById("node_a"));

  // Nodes still collide but don't create a new scene_node.
  CollisionChecker::Instance().Dirty(*node_a);
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(
      1, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));

  // Move node_a away (50,0) and rerun collision checking. Nodes no longer
  // collide. No new sprite_c node is created.
  node_a->mutable_position()->set_x(50);
  CollisionChecker::Instance().Dirty(*node_a);
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(
      1, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));

  // Bring back node_a to its original positon (0,0) and rerun collision
  // checking. Nodes a and bcollide creating a new sprite_c node.
  node_a->mutable_position()->set_x(0);
  CollisionChecker::Instance().Dirty(*node_a);
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_EQ(
      2, ranges::count_if(scene_manager_->GetSceneNodesBySpriteId("sprite_c"),
                          count_all));
}

TEST_F(CollisionCheckerTest, NodesDetachingExecutingTheirAction) {
  CollisionChecker::Instance().RegisterDetachment(
      ParseProto<CollisionAction>(R"(
        scene_node_id: [ 'node_a', 'node_b' ]
        action {
          create_scene_node {
            scene_node { 
              id: 'created_node'
              sprite_id: 'sprite_c'
            }
          }
        })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Node collision has no registered action no node is created.
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);

  // Break constness rules locally, to allow modify node's position for testing.
  SceneNode* node_a =
      const_cast<SceneNode*>(scene_manager_->GetSceneNodeById("node_a"));

  CollisionChecker::Instance().Dirty(*node_a);
  CollisionChecker::Instance().CheckCollisions();

  // Nodes still collide and still don't create a new scene_node.
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);

  node_a->mutable_position()->set_x(50);
  CollisionChecker::Instance().Dirty(*node_a);
  CollisionChecker::Instance().CheckCollisions();

  // Nodes detached causing creation of new node.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    )"))));
}

TEST_F(CollisionCheckerTest, MultipleCollisionActionsForSameNodePair) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'node_c'
          sprite_id: 'sprite_c'
        }
      }
    })"));
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: 'node_a'
    sprite_id: 'sprite_b'
    action {
      create_scene_node {
        scene_node { 
          id: 'node_d'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_c"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'node_c'
                                    sprite_id: 'sprite_c')"))));
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_d"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'node_d'
                                    sprite_id: 'sprite_c')"))));
}

TEST_F(CollisionCheckerTest,
       CollisionResultsToNewNodeWhichAlsoTriggersCollision) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'node_c'
          sprite_id: 'sprite_c'
        }
      }
    })"));
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: 'node_a'
    sprite_id: 'sprite_c'
    action {
      create_scene_node {
        scene_node { 
          id: 'node_d'
          sprite_id: 'sprite_a'
          position { x: 50  y: 50 }
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_c"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'node_c'
                                    sprite_id: 'sprite_c')"))));
  EXPECT_THAT(scene_manager_->GetSceneNodeById("node_d"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'node_d'
                                    sprite_id: 'sprite_a'
                                    position { x: 50  y: 50 })"))));
}

TEST_F(CollisionCheckerTest, SingleSpriteCollisionsAllowed) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_a'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c')"))));
}

TEST_F(CollisionCheckerTest, SameSpriteCollisionsAllowed) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_a' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_a'
    position { x: 5  y: 5 })"));

  CollisionChecker::Instance().CheckCollisions();

  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c')"))));
}

TEST_F(CollisionCheckerTest, NodesCannotCollideByThemselves) {
  CollisionChecker::Instance().RegisterCollision(ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a' ]
    sprite_id: [ 'sprite_a' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })"));

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })"));

  CollisionChecker::Instance().CheckCollisions();

  // Even though node_a is of sprite_a it cannot collide by itself.
  EXPECT_EQ(scene_manager_->GetSceneNodeById("created_node"), nullptr);
}

class PixelCollisionTest : public testing::Test {
 protected:
  void SetUp() override {
    //  LHS  -  RHS
    // 10000   11000
    // 10011   01100
    // 11110   11111
    std::string tmp = "100001001111110";
    lhs_mask_ = boost::dynamic_bitset<>(std::string(tmp.rbegin(), tmp.rend()));
    tmp = "110000110011111";
    rhs_mask_ = boost::dynamic_bitset<>(std::string(tmp.rbegin(), tmp.rend()));
  }

  boost::dynamic_bitset<> lhs_mask_;
  boost::dynamic_bitset<> rhs_mask_;
};

TEST_F(PixelCollisionTest, SpritesNextToEachOtherNoCollision) {
  Box lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");
  EXPECT_FALSE(
      internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

TEST_F(PixelCollisionTest, SpriteFitsInsideTheOtherNoCollision) {
  Box lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 4  top: 0  width: 5  height: 3");
  EXPECT_FALSE(
      internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

TEST_F(PixelCollisionTest, SpritesCollide) {
  Box lhs = ParseProto<Box>("left: 0  top: 0  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 3  top: 0  width: 5  height: 3");
  EXPECT_TRUE(internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

TEST_F(PixelCollisionTest, BoxesOverlapFromTopButPixelsDontCollide) {
  Box lhs = ParseProto<Box>("left: 4  top: 2  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");
  EXPECT_FALSE(
      internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));

  // Moving lhs one pixel to the right causes a collision.
  lhs = ParseProto<Box>("left: 5  top: 2  width: 5  height: 3");
  EXPECT_TRUE(internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

TEST_F(PixelCollisionTest, SpritesWithNegativePositionsThatDontCollide) {
  Box lhs = ParseProto<Box>("left: -2  top: -2  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 5  top: 0  width: 5  height: 3");
  EXPECT_FALSE(
      internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

TEST_F(PixelCollisionTest, SpritesWithNegativePositionsThatCollide) {
  Box lhs = ParseProto<Box>("left: -1  top: -2  width: 5  height: 3");
  Box rhs = ParseProto<Box>("left: 1  top: 0  width: 5  height: 3");
  EXPECT_TRUE(internal::SceneNodePixelsCollide(lhs, rhs, lhs_mask_, rhs_mask_));
}

}  // namespace troll
