#ifndef TROLL_CORE_EXECUTION_CONTEXT_H_
#define TROLL_CORE_EXECUTION_CONTEXT_H_

#include <string>
#include <vector>

namespace troll {

class ExecutionContext {
 public:
  static ExecutionContext& Instance() {
    static ExecutionContext singleton;
    return singleton;
  }

  const std::vector<std::string>& scene_nodes() const {
    return scene_node_ids_;
  }

 private:
  void AddSceneNodes(const std::vector<std::string>& scene_node_ids);
  void ClearSceneNodes();

  std::vector<std::string> scene_node_ids_;

  friend class CollisionChecker;
};

}  // namespace troll

#endif  // TROLL_CORE_EXECUTION_CONTEXT_H_
