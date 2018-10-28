#include "core/execution-context.h"

namespace troll {

void ExecutionContext::AddSceneNodes(
    const std::vector<std::string>& scene_node_ids) {
  scene_node_ids_ = scene_node_ids;
}

void ExecutionContext::ClearSceneNodes() { scene_node_ids_.clear(); }

}  // namespace troll
