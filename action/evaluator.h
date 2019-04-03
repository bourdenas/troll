#ifndef TROLL_ACTION_EVALUATOR_H_
#define TROLL_ACTION_EVALUATOR_H_

#include "core/core.h"
#include "proto/query.pb.h"

namespace troll {

class Evaluator {
 public:
  Evaluator() = default;
  virtual ~Evaluator() = default;

  virtual Response Eval(const Query& query) const = 0;
};

class SceneNodeEvaluator : public Evaluator {
 public:
  SceneNodeEvaluator(Core* core) : core_(core) {}

  virtual Response Eval(const Query& query) const;

 private:
  Core* core_;
};

class SceneNodeOverlapEvaluator : public Evaluator {
 public:
  SceneNodeOverlapEvaluator(Core* core) : core_(core) {}

  virtual Response Eval(const Query& query) const;

 private:
  Core* core_;
};

}  // namespace troll

#endif  // TROLL_ACTION_EVALUATOR_H_
