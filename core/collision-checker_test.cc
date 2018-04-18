#include "core/collision-checker.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
    scene_manager_ = new SceneManager(Renderer());
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
        width: 2  height: 2
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

TEST_F(CollisionCheckerTest, NodesTouchingTriggersCollision) {
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

  // Nodes A and B only touch theirs bounding box edges. Touching is collision.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    )"))));
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

  scene_manager_->AddSceneNode(ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 3  y: 3 })"));

  CollisionChecker::Instance().CheckCollisions();

  // All three nodes collide together. A overlaps with B, B overlaps with C and
  // C is also fully inside A. The action will be executed 3 times, once for
  // each pair.
  // TODO(bourdenas): create_scene_node is not a repeatable action because ids
  // are colliding, so this creates only one scene node. Need to find a
  // repeatable action to test here.
  EXPECT_THAT(scene_manager_->GetSceneNodeById("created_node"),
              Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                    id: 'created_node'
                                    sprite_id: 'sprite_c'
                                    )"))));
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

  // Both node_a and node_b are of same sprite and collide triggering the
  // action.
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

}  // namespace troll
