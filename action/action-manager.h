#ifndef TROLL_ACTION_ACTION_MANAGER_H_
#define TROLL_ACTION_ACTION_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "action/executor.h"
#include "proto/action.pb.h"

namespace troll {

class ActionManager {
 public:
  static ActionManager& Instance() {
    static ActionManager singleton;
    return singleton;
  }

  void Init();

  void Execute(const Action& action);
  Action Reverse(const Action& action);

  ActionManager(const ActionManager&) = delete;
  ActionManager& operator=(const ActionManager&) = delete;

 private:
  ActionManager() = default;
  ~ActionManager() = default;

  std::unordered_map<Action::ActionCase, std::unique_ptr<Executor>> executors_;
};

}  // namespace troll

#endif  // TROLL_ACTION_ACTION_MANAGER_H_