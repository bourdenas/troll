#ifndef TROLL_ACTION_ACTION_MANAGER_H_
#define TROLL_ACTION_ACTION_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "action/executor.h"
#include "core/core.h"
#include "proto/action.pb.h"

namespace troll {

class ActionManager {
 public:
  ActionManager(Core* core);
  ~ActionManager() = default;

  void Execute(const Action& action) const;
  Action Reverse(const Action& action) const;

  ActionManager(const ActionManager&) = delete;
  ActionManager& operator=(const ActionManager&) = delete;

 private:
  std::unordered_map<Action::ActionCase, std::unique_ptr<Executor>> executors_;
};

}  // namespace troll

#endif  // TROLL_ACTION_ACTION_MANAGER_H_
