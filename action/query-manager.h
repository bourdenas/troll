#ifndef TROLL_ACTION_QUERY_MANAGER_H_
#define TROLL_ACTION_QUERY_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "action/evaluator.h"
#include "core/core.h"
#include "proto/query.pb.h"

namespace troll {

class QueryManager {
 public:
  QueryManager(Core* core);
  ~QueryManager() = default;

  Response Eval(const Query& query) const;

  QueryManager(const QueryManager&) = delete;
  QueryManager& operator=(const QueryManager&) = delete;

 private:
  std::unordered_map<Query::QueryCase, std::unique_ptr<Evaluator>> evaluators_;
};

}  // namespace troll

#endif  // TROLL_ACTION_QUERY_MANAGER_H_
