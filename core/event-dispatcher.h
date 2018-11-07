#ifndef TROLL_CORE_EVENT_DISPATCHER_H_
#define TROLL_CORE_EVENT_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace troll {

using EventHandler = std::function<void()>;

// Module that handles events in the system. When an event is emitted the
// associated handlers are activated.
//
// Event defintions can be found in "core/events.h".
class EventDispatcher {
 public:
  static EventDispatcher& Instance(void) {
    static EventDispatcher singleton;
    return singleton;
  }

  // Registers an event_id with a handler, calling it when the event is emitted.
  void Register(const std::string& event_id, const EventHandler& handler);

  // Triggers event handlers of input event. Event handlers are not called
  // immediately but all handlers whose events fired are batch executed when
  // ProcessTriggeredEvents() is called.
  void Emit(const std::string& event_id);

  // Activates all fired events.
  void ProcessTriggeredEvents();

 private:
  EventDispatcher() = default;
  ~EventDispatcher() = default;

  std::multimap<std::string, EventHandler> event_registry_;
  std::vector<std::string> triggered_events_;
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_DISPATCHER_H_
