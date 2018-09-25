#ifndef TROLL_CORE_EVENT_DISPATCHER_H_
#define TROLL_CORE_EVENT_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>

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

  // Activates all event handlers of input event.
  void Emit(const std::string& event_id);

 private:
  EventDispatcher() = default;
  ~EventDispatcher() = default;

  std::multimap<std::string, EventHandler> event_registry_;
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_DISPATCHER_H_
