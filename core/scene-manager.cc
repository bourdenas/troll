#include "core/scene-manager.h"

#include <glog/logging.h>
#include <range/v3/action/sort.hpp>

#include "action/action-manager.h"
#include "core/collision-checker.h"
#include "core/geometry.h"
#include "core/resource-manager.h"

namespace troll {

void SceneManager::SetupScene(const Scene& scene) {
  scene_ = scene;
  renderer_.ClearScreen();
  renderer_.FillColour(scene_.bitmap_config().background_colour(),
                       scene_.viewport());

  if (scene_.bitmap_config().has_bitmap()) {
    renderer_.BlitTexture(
        ResourceManager::Instance().GetTexture(scene_.bitmap_config().bitmap()),
        Box(), Box());
  }

  CollisionChecker::Instance().Init();
  for (const auto& node : scene_.scene_node()) {
    AddSceneNode(node);
  }

  for (const auto& action : scene_.on_init()) {
    ActionManager::Instance().Execute(action);
  }
}

void SceneManager::AddSceneNode(const SceneNode& node) {
  const auto res = scene_nodes_.emplace(node.id(), node);
  LOG_IF(ERROR, !res.second) << "AddSceneNode() SceneNode with id='"
                             << node.id() << "' already exists.";

  const auto it = res.first;
  Dirty(it->second);
}

void SceneManager::RemoveSceneNode(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  LOG_IF(ERROR, it == scene_nodes_.end())
      << "RemoveSceneNode() cannot find SceneNode with id='" << id << "'.";

  dirty_boxes_.push_back(GetSceneNodeBoundingBox(it->second));
  dead_scene_nodes_.insert(id);
}

SceneNode* SceneManager::GetSceneNodeById(const std::string& id) {
  const auto it = scene_nodes_.find(id);
  return it != scene_nodes_.end() ? &it->second : nullptr;
}

SceneNode* SceneManager::GetSceneNodeAt(const Vector& at) {
  auto nodes = scene_nodes_ | ranges::view::values;
  auto it = std::find_if(
      nodes.begin(), nodes.end(), [this, &at](const SceneNode& node) {
        return geo::Contains(GetSceneNodeBoundingBox(node), at);
      });
  return it != nodes.end() ? &(*it) : nullptr;
}

Box SceneManager::GetSceneNodeBoundingBox(const SceneNode& node) const {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  auto bounding_box = sprite.film(node.frame_index());
  bounding_box.set_left(node.position().x());
  bounding_box.set_top(node.position().y());
  return bounding_box;
}

void SceneManager::SetViewport(const Box& view) {
  viewport_ = view;
  // TODO(bourdenas): Render everything.
}

void SceneManager::ScrollViewport(const Vector& by) {
  // TODO(bourdenas): translate(viewport, by)
  // TODO(bourdenas): Render everything.
}

void SceneManager::Render() {
  // Collect scene nodes that overlap with dirty bounding boxes.
  std::unordered_set<const SceneNode*> dirty_nodes;
  for (int i = 0; i < dirty_boxes_.size(); ++i) {
    auto& overlap_nodes =
        scene_nodes_ | ranges::view::values |
        ranges::view::filter([&dirty_nodes](const SceneNode& node) {
          return dirty_nodes.find(&node) == dirty_nodes.end();
        }) |
        ranges::view::filter([this, i](const SceneNode& node) {
          return geo::Collide(dirty_boxes_[i], GetSceneNodeBoundingBox(node));
        });

    for (const auto& node : overlap_nodes) {
      dirty_nodes.insert(&node);
      dirty_boxes_.push_back(GetSceneNodeBoundingBox(node));
    }
  }

  // Render behind bounding boxes.
  for (const auto& box : dirty_boxes_) {
    renderer_.FillColour(scene_.bitmap_config().background_colour(), box);
  }
  dirty_boxes_.clear();

  // Render dirty nodes.
  std::vector<const SceneNode*> z_ordered_nodes =
      dirty_nodes | ranges::view::filter([this](const SceneNode* node) {
        return dead_scene_nodes_.find(node->id()) == dead_scene_nodes_.end();
      });
  z_ordered_nodes |=
      ranges::action::sort([](const SceneNode* lhs, const SceneNode* rhs) {
        return lhs->position().z() < rhs->position().z();
      });
  for (const auto* node : z_ordered_nodes) {
    BlitSceneNode(*node);
  }

  renderer_.Flip();

  CleanUpDeletedSceneNodes();
}

void SceneManager::Dirty(const SceneNode& scene_node) {
  dirty_boxes_.push_back(GetSceneNodeBoundingBox(scene_node));
  CollisionChecker::Instance().Dirty(scene_node);
}

void SceneManager::BlitSceneNode(const SceneNode& node) const {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  const auto& bounding_box = sprite.film(node.frame_index());
  Box destination = bounding_box;
  destination.set_left(node.position().x());
  destination.set_top(node.position().y());

  renderer_.BlitTexture(
      ResourceManager::Instance().GetTexture(sprite.resource()), bounding_box,
      destination);
}

void SceneManager::CleanUpDeletedSceneNodes() {
  for (const auto& id : dead_scene_nodes_) {
    const auto it = scene_nodes_.find(id);
    LOG_IF(ERROR, it == scene_nodes_.end())
        << "CleanUpDeletedSceneNodes() SceneNode with id='" << id
        << "' was not found.";

    scene_nodes_.erase(it);
  }
  dead_scene_nodes_.clear();
}

}  // namespace troll
