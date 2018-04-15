#include "core/collision-checker.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "action/action-manager.h"
#include "core/troll-core.h"
#include "proto/scene-node.pb.h"
#include "troll-test/test-util.h"
#include "troll-test/testing-resource-manager.h"

namespace troll {

using testing::UnorderedElementsAre;
using testing::Pointee;

class CollisionCheckerTest : public testing::Test {
 protected:
  virtual void SetUp() {
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

  const std::unordered_set<const SceneNode*>& dirty_nodes() const {
    return CollisionChecker::Instance().dirty_nodes_;
  }

  SceneManager* scene_manager_ = nullptr;
};

TEST_F(CollisionCheckerTest, CollisionCheckerIsBoringWithoutNodesCollisions) {
  CollisionChecker::Instance().CheckCollisions();
}

TEST_F(CollisionCheckerTest, CollisionCheckerStillBoringWithoutNodes) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node {
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");

  CollisionChecker::Instance().RegisterCollision(collision_action);

  CollisionChecker::Instance().CheckCollisions();
  EXPECT_TRUE(dirty_nodes().empty());
}

TEST_F(CollisionCheckerTest,
       CollisionCheckerStillBoringWithoutCollisionActions) {
  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_b = ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_b);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_b))));
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_TRUE(dirty_nodes().empty());
}

TEST_F(CollisionCheckerTest, NodesDontCollideNoActionExecuted) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");
  CollisionChecker::Instance().RegisterCollision(collision_action);

  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_b = ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 20  y: 20 })");
  scene_manager_->AddSceneNode(node_b);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_b))));

  CollisionChecker::Instance().CheckCollisions();
  EXPECT_TRUE(dirty_nodes().empty());
}

TEST_F(CollisionCheckerTest, NodesOverlapExecutingTheirAction) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");
  CollisionChecker::Instance().RegisterCollision(collision_action);

  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_b = ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })");
  scene_manager_->AddSceneNode(node_b);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_b))));

  CollisionChecker::Instance().CheckCollisions();
  // CreateSceneNode action executes due to collision and creates new node,
  // which should appear on the dirty nodes for collisions because it is new.
  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                                         id: 'created_node'
                                                         sprite_id: 'sprite_c'
                                                         )")))));
}

TEST_F(CollisionCheckerTest, NodesTouchingTriggersCollision) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");
  CollisionChecker::Instance().RegisterCollision(collision_action);

  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_b = ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 5  y: 5 })");
  scene_manager_->AddSceneNode(node_b);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_b))));

  // Nodes A and B only touch theirs bounding box edges. This is still a
  // collision.
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                                         id: 'created_node'
                                                         sprite_id: 'sprite_c'
                                                         )")))));
}

TEST_F(CollisionCheckerTest, MultiNodeCollisionsArePairwise) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    scene_node_id: [ 'node_a', 'node_b', 'node_c' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");
  CollisionChecker::Instance().RegisterCollision(collision_action);

  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_b = ParseProto<SceneNode>(R"(
    id: 'node_b'
    sprite_id: 'sprite_b'
    position { x: 20  y: 20 })");
  scene_manager_->AddSceneNode(node_b);

  const auto node_c = ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 25  y: 25 })");
  scene_manager_->AddSceneNode(node_c);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_b)),
                                   Pointee(EqualsProto(node_c))));

  // Only node_b and node_c collide (C is inside B). The action will be executed
  // once.
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(ParseProto<SceneNode>(R"(
                                                         id: 'created_node'
                                                         sprite_id: 'sprite_c'
                                                         )")))));
}

TEST_F(CollisionCheckerTest, NodesCollideWithoutMatchingSprites) {
  const auto collision_action = ParseProto<CollisionAction>(R"(
    sprite_id: [ 'sprite_a', 'sprite_b' ]
    action {
      create_scene_node {
        scene_node { 
          id: 'created_node'
          sprite_id: 'sprite_c'
        }
      }
    })");
  CollisionChecker::Instance().RegisterCollision(collision_action);

  const auto node_a = ParseProto<SceneNode>(R"(
    id: 'node_a'
    sprite_id: 'sprite_a'
    position { x: 0  y: 0 })");
  scene_manager_->AddSceneNode(node_a);

  const auto node_c = ParseProto<SceneNode>(R"(
    id: 'node_c'
    sprite_id: 'sprite_c'
    position { x: 5  y: 5 })");
  scene_manager_->AddSceneNode(node_c);

  EXPECT_THAT(dirty_nodes(),
              UnorderedElementsAre(Pointee(EqualsProto(node_a)),
                                   Pointee(EqualsProto(node_c))));

  // Even though nodes collide they don't have a registered action for their
  // collision.
  CollisionChecker::Instance().CheckCollisions();
  EXPECT_TRUE(dirty_nodes().empty());
}

}  // namespace troll
