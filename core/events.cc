#include "core/events.h"

#include <absl/strings/str_join.h>

namespace troll {

Event Events::OnAnimationScriptTermination(const std::string& scene_node_id,
                                           const std::string& script_id) {
  Event event;
  event.set_event_id(
      absl::StrJoin({scene_node_id, script_id, std::string("done")}, "."));
  return event;
}

Event Events::OnAnimationScriptRewind(const std::string& scene_node_id,
                                      const std::string& script_id) {
  Event event;
  event.set_event_id(
      absl::StrJoin({scene_node_id, script_id, std::string("rewind")}, "."));
  return event;
}

Event Events::OnAnimationScriptPartTermination(
    const std::string& scene_node_id, const std::string& script_id,
    const std::string& animation_id) {
  Event event;
  event.set_event_id(absl::StrJoin(
      {scene_node_id, script_id, animation_id, std::string("done")}, "."));
  return event;
}

}  // namespace troll
