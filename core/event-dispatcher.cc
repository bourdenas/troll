#include "core/event-dispatcher.h"

#include <absl/strings/str_join.h>

namespace troll {

void EventDispatcher::Register(const std::string& event_id,
                               const EventHandler& handler) {
  event_registry_.emplace(event_id, handler);
}

void EventDispatcher::Emit(const std::string& event_id) {
  for (auto it = event_registry_.lower_bound(event_id);
       it != event_registry_.upper_bound(event_id); ++it) {
    it->second();
  }
  event_registry_.erase(event_id);
}

}  // namespace troll
