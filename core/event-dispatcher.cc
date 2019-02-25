#include "core/event-dispatcher.h"

#include <iterator>
#include <vector>

#include <range/v3/action/push_back.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/view/transform.hpp>

namespace troll {

namespace {
int kHandlerId = 0;
}  // namespace

int EventDispatcher::Register(const std::string& event_id,
                              const EventHandler& handler) {
  const int handler_id = kHandlerId++;
  event_registry_[event_id].push_back(HandlerInfo{handler_id, false, handler});
  return handler_id;
}

int EventDispatcher::RegisterPermanent(const std::string& event_id,
                                       const EventHandler& handler) {
  const int handler_id = kHandlerId++;
  event_registry_[event_id].push_back(HandlerInfo{handler_id, true, handler});
  return handler_id;
}

void EventDispatcher::Unregister(const std::string& event_id, int handler_id) {
  const auto it = event_registry_.find(event_id);
  if (it == event_registry_.end()) return;

  auto& event_handlers = it->second;
  const auto vec_it = std::find_if(
      event_handlers.begin(), event_handlers.end(),
      [handler_id](const HandlerInfo& info) { return info.id == handler_id; });
  if (vec_it == event_handlers.end()) return;

  event_handlers.erase(vec_it);
  if (event_handlers.empty()) {
    event_registry_.erase(it);
  }
}

void EventDispatcher::Emit(const std::string& event_id) {
  triggered_events_.push_back(event_id);
}

void EventDispatcher::ProcessTriggeredEvents() {
  std::vector<EventHandler> handlers;
  for (const auto& event_id : triggered_events_) {
    auto it = event_registry_.find(event_id);
    if (it == event_registry_.end()) continue;

    // Collect activated event handlers.
    handlers |= ranges::push_back(
        it->second |
        ranges::view::transform([](const auto& info) { return info.handler; }));

    // Remove non-permanent event handlers.
    const auto erase_it =
        std::remove_if(it->second.begin(), it->second.end(),
                       [](const auto& info) { return !info.permanent; });
    it->second.erase(erase_it, it->second.end());
    if (it->second.empty()) {
      event_registry_.erase(it);
    }
  }
  triggered_events_.clear();

  // Delayed call of handlers because they cause side-effects that might
  // register new events.
  for (const auto& handler : handlers) {
    handler();
  }
}

}  // namespace troll
