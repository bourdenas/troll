#include "core/events.h"

#include <absl/strings/str_join.h>

namespace troll {

std::string Events::OnAnimationScriptTermination(
    const std::string& scene_node_id, const std::string& script_id) {
  return absl::StrJoin({scene_node_id, script_id, std::string("done")}, ".");
}

}  // namespace troll