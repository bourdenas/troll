#include "action/query-manager.h"

#include <glog/logging.h>

namespace troll {

QueryManager::QueryManager(Core* core) {
  evaluators_.emplace(Query::kSceneNode,
                      std::make_unique<SceneNodeEvaluator>(core));
  evaluators_.emplace(Query::kSceneNodeOverlap,
                      std::make_unique<SceneNodeOverlapEvaluator>(core));
}

Response QueryManager::Eval(const Query& query) const {
  const auto type = query.Query_case();
  const auto it = evaluators_.find(type);
  if (it == evaluators_.end()) {
    LOG(ERROR)
        << "Query: " << query.DebugString()
        << " is not registered with QueryManager and has no valid Evaluator.";
    return Response();
  }
  return it->second->Eval(query);
}

}  // namespace troll
