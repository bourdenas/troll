#ifndef TROLL_CORE_EVENT_DISPATCHER_H_
#define TROLL_CORE_EVENT_DISPATCHER_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "proto/event.pb.h"

namespace troll {

using EventHandler = std::function<void(const Event&)>;

// Module that handles events in the system. When an event is emitted the
// associated handlers are activated.
//
// Event definitions can be found in "core/events.h".
class EventDispatcher {
 public:
  EventDispatcher() = default;
  ~EventDispatcher() = default;

  // Registers an event_id with a handler, calling it when the event is emitted.
  // Returns a handler id that can be used in combination with the event_id to
  // unregister the handler. The handler is called only once regardless of how
  // many times the event is triggered.
  int Register(const std::string& event_id, const EventHandler& handler);

  // Registers an event_id with a handler, calling it when the event is emitted.
  // Returns a handler id that can be used in combination with the event_id to
  // unregister the handler. The handler will be called every time the event is
  // triggered until it gets unregistered.
  int RegisterPermanent(const std::string& event_id,
                        const EventHandler& handler);

  // Unregister a handler of an event using its handler id returned by handler
  // registration.
  void Unregister(const std::string& event_id, int handler_id);

  // Triggers event handlers of input event. Event handlers are not called
  // immediately but all handlers whose events fired are batch executed when
  // ProcessTriggeredEvents() is called.
  void Emit(const Event& event_id);

  // Activates all fired events.
  void ProcessTriggeredEvents();

  EventDispatcher(const EventDispatcher&) = delete;
  EventDispatcher& operator=(const EventDispatcher&) = delete;

 private:
  struct HandlerInfo {
    int handler_id;
    bool permanent;
    EventHandler handler;
  };

  std::unordered_map<std::string, std::vector<HandlerInfo>> event_registry_;
  std::vector<Event> triggered_events_;
};

}  // namespace troll

#endif  // TROLL_CORE_EVENT_DISPATCHER_H_
