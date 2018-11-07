#include "core/event-dispatcher.h"

#include <iterator>
#include <vector>

#include <absl/strings/str_join.h>

namespace troll {

void EventDispatcher::Register(const std::string& event_id,
                               const EventHandler& handler) {
  event_registry_.emplace(event_id, handler);
}

void EventDispatcher::Emit(const std::string& event_id) {
  triggered_events_.push_back(event_id);
}

void EventDispatcher::ProcessTriggeredEvents() {
  std::vector<EventHandler> handlers;
  for (const auto& event_id : triggered_events_) {
    for (auto it = event_registry_.lower_bound(event_id);
         it != event_registry_.upper_bound(event_id); ++it) {
      handlers.push_back(it->second);
    }
    event_registry_.erase(event_id);
  }
  triggered_events_.clear();

  // Delayed call of handlers because they cause side-effects that might
  // register new events.
  for (const auto& handler : handlers) {
    handler();
  }
}

}  // namespace troll
