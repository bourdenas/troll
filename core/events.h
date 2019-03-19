#ifndef TROLL_CORE_EVENT_H_
#define TROLL_CORE_EVENT_H_

#include <string>

namespace troll {

struct Events {
  // Returns event_id of animation script termination for a scene node.
  static std::string OnAnimationScriptTermination(
      const std::string& scene_node_id, const std::string& script_id);

  // Returns event_id of animation script finished for a scene node and rewinds.
  static std::string OnAnimationScriptRewind(const std::string& scene_node_id,
                                             const std::string& script_id);

  // Returns event_id of animation part of a script terminates.
  static std::string OnAnimationScriptPartTermination(
      const std::string& scene_node_id, const std::string& script_id,
      const std::string& animation_id);
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_H_
