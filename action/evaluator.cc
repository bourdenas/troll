#include "action/evaluator.h"

#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include "core/geometry.h"
#include "core/scene-manager.h"

namespace troll {

Response SceneNodeEvaluator::Eval(const Query& query) const {
  Response response;

  const auto node_ids = core_->scene_manager()->GetSceneNodesByPattern(
      query.scene_node().pattern());
  auto&& nodes = node_ids |
                 ranges::view::transform([this](const std::string& node_id) {
                   return core_->scene_manager()->GetSceneNodeById(node_id);
                 }) |
                 ranges::view::remove_if(
                     [](const SceneNode* node) { return node == nullptr; });

  for (const auto* node : nodes) {
    *response.mutable_scene_nodes()->add_scene_node() = *node;
  }

  return response;
}

Response SceneNodeOverlapEvaluator::Eval(const Query& query) const {
  Response response;

  const auto* first_node = core_->scene_manager()->GetSceneNodeById(
      query.scene_node_overlap().first_node_id());
  const auto* second_node = core_->scene_manager()->GetSceneNodeById(
      query.scene_node_overlap().second_node_id());
  if (first_node == nullptr || second_node == nullptr) return response;

  const auto first_aabb =
      core_->scene_manager()->GetSceneNodeBoundingBox(*first_node);
  const auto second_aabb =
      core_->scene_manager()->GetSceneNodeBoundingBox(*second_node);

  if (geo::Collide(first_aabb, second_aabb)) {
    *response.mutable_overlap() = geo::Intersection(first_aabb, second_aabb);
  }

  return response;
}

}  // namespace troll
