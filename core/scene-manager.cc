#include "core/scene-manager.h"

#include <glog/logging.h>
#include <range/v3/action/push_back.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"
#include "core/collision-checker.h"
#include "core/geometry.h"
#include "core/resource-manager.h"
#include "sdl/renderer.h"

namespace troll {

void SceneManager::SetupScene(const Scene& scene) {
  scene_ = scene;
  Renderer::Instance().ClearScreen();
  Renderer::Instance().FillColour(scene_.bitmap_config().background_colour(),
                                  scene_.viewport());

  if (scene_.bitmap_config().has_bitmap()) {
    Renderer::Instance().BlitTexture(
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

void SceneManager::Dirty(const SceneNode& scene_node) {
  dirty_boxes_.push_back(GetSceneNodeBoundingBox(scene_node));
  CollisionChecker::Instance().Dirty(scene_node);
}

const SceneNode* SceneManager::GetSceneNodeById(const std::string& id) const {
  const auto it = scene_nodes_.find(id);
  return it != scene_nodes_.end() ? &it->second : nullptr;
}

SceneNode* SceneManager::GetSceneNodeById(const std::string& id) {
  return const_cast<SceneNode*>(
      static_cast<const SceneManager*>(this)->GetSceneNodeById(id));
}

std::vector<std::string> SceneManager::GetSceneNodesAt(const Vector& at) const {
  std::vector<std::string> filtered_nodes;
  filtered_nodes |= ranges::push_back(
      scene_nodes_ | ranges::view::values |
      ranges::view::filter([&at](const SceneNode& node) {
        return geo::Contains(SceneManager::GetSceneNodeBoundingBox(node), at);
      }) |
      ranges::view::transform([](const SceneNode& node) { return node.id(); }));
  return filtered_nodes;
}

std::vector<std::string> SceneManager::GetSceneNodesBySpriteId(
    const std::string& sprite_id) const {
  std::vector<std::string> filtered_nodes;
  filtered_nodes |= ranges::push_back(
      scene_nodes_ | ranges::view::values |
      ranges::view::filter([&sprite_id](const SceneNode& node) {
        return node.sprite_id() == sprite_id;
      }) |
      ranges::view::transform([](const SceneNode& node) { return node.id(); }));
  return filtered_nodes;
}

std::vector<std::string> SceneManager::GetSceneNodesByPattern(
    const SceneNode& pattern) const {
  std::vector<std::string> filtered_nodes;
  filtered_nodes |= ranges::push_back(
      scene_nodes_ | ranges::view::values |
      ranges::view::filter([pattern](const SceneNode& node) {
        return SceneManager::NodePatternMatching(pattern, node);
      }) |
      ranges::view::transform([](const SceneNode& node) { return node.id(); }));
  return filtered_nodes;
}

std::vector<std::string> SceneManager::GetSceneNodesByPattern(
    const SceneNode& pattern, const std::vector<std::string>& node_ids) const {
  std::vector<std::string> filtered_nodes;
  filtered_nodes |= ranges::push_back(
      node_ids | ranges::view::transform([this](const std::string& id) {
        return GetSceneNodeById(id);
      }) |
      ranges::view::filter([pattern](const SceneNode* node) {
        return node != nullptr &&
               SceneManager::NodePatternMatching(pattern, *node);
      }) |
      ranges::view::transform(
          [](const SceneNode* node) { return node->id(); }));
  return filtered_nodes;
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
    Renderer::Instance().FillColour(scene_.bitmap_config().background_colour(),
                                    box);
  }
  dirty_boxes_.clear();

  // Render dirty nodes.
  std::vector<const SceneNode*> z_ordered_nodes =
      dirty_nodes | ranges::view::filter([this](const SceneNode* node) {
        return node->visible();
      }) |
      ranges::view::filter([this](const SceneNode* node) {
        return dead_scene_nodes_.find(node->id()) == dead_scene_nodes_.end();
      });
  z_ordered_nodes |=
      ranges::action::sort([](const SceneNode* lhs, const SceneNode* rhs) {
        return lhs->position().z() < rhs->position().z();
      });
  for (const auto* node : z_ordered_nodes) {
    BlitSceneNode(*node);
  }

  Renderer::Instance().Flip();

  CleanUpDeletedSceneNodes();
}

Box SceneManager::GetSceneNodeBoundingBox(const SceneNode& node) {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  auto bounding_box = sprite.film(node.frame_index());
  bounding_box.set_left(node.position().x());
  bounding_box.set_top(node.position().y());
  return bounding_box;
}

void SceneManager::BlitSceneNode(const SceneNode& node) const {
  const auto& sprite = ResourceManager::Instance().GetSprite(node.sprite_id());

  const auto& bounding_box = sprite.film(node.frame_index());
  Box destination = bounding_box;
  destination.set_left(node.position().x());
  destination.set_top(node.position().y());

  Renderer::Instance().BlitTexture(
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

bool SceneManager::NodePatternMatching(const SceneNode& pattern,
                                       const SceneNode& node) {
  if (pattern.has_id() && node.id() != pattern.id()) return false;
  if (pattern.has_sprite_id() && node.sprite_id() != pattern.sprite_id()) {
    return false;
  }
  if (pattern.has_frame_index() &&
      node.frame_index() != pattern.frame_index()) {
    return false;
  }
  if (pattern.has_visible() && node.visible() != pattern.visible()) {
    return false;
  }
  return true;
}

}  // namespace troll
