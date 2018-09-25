#ifndef TROLL_CORE_EVENT_H_
#define TROLL_CORE_EVENT_H_

#include <string>

namespace troll {

struct Events {
  // Returns event_id of animation script termination for a scene node.
  static std::string OnAnimationScriptTermination(
      const std::string& scene_node_id, const std::string& script_id);
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_H_
