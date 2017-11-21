#ifndef TROLL_CORE_ACTION_MANAGER_H_
#define TROLL_CORE_ACTION_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "core/executor.h"
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

 private:
  ActionManager() = default;
  ActionManager(const ActionManager&) = delete;
  ~ActionManager() = default;

  std::unordered_map<Action::ActionCase, std::unique_ptr<Executor>> executors_;
};

}  // namespace troll

#endif  // TROLL_CORE_ACTION_MANAGER_H_
