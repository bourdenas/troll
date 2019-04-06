#ifndef TROLL_CORE_EVENT_H_
#define TROLL_CORE_EVENT_H_

#include <string>

#include "proto/event.pb.h"

namespace troll {

struct Events {
  // Returns event_id of animation script termination for a scene node.
  static Event OnAnimationScriptTermination(const std::string& scene_node_id,
                                            const std::string& script_id);

  // Returns event_id of animation script finished for a scene node and rewinds.
  static Event OnAnimationScriptRewind(const std::string& scene_node_id,
                                       const std::string& script_id);

  // Returns event_id of animation part of a script terminates.
  static Event OnAnimationScriptPartTermination(
      const std::string& scene_node_id, const std::string& script_id,
      const std::string& animation_id);
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_H_
