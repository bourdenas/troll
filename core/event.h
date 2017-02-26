#ifndef TROLL_CORE_WORLD_EVENT_H_
#define TROLL_CORE_WORLD_EVENT_H_

#include <map>
#include <string>

#include "action.h"
#include "world-element.h"

namespace troll {

// Event represents a trigger in the system that can used to signal actions.ζ`
// Since it is a world element it is a first class citizen in the system that
// can have properties attached.
class Event : public WorldElement {
 public:
  Event(const std::string& id) : WorldElement(id) {}
  ~Event() override = default;

  void Emit() const;
};

// EventHandlers are contexts that contain event to action lists mappings. When
// an Event is dispatched that exists in their registry they activate the
// associated action list.
class EventHandler {
 public:
  EventHandler() = default;
  virtual ~EventHandler() = default;

  void Activate();
  void Deactivate();

  void Register(const std::string& evt_id, const Action& action);
  void Handle(const Event& evt) const;

 private:
  // Event ID to Action mapping.
  std::multimap<std::string, Action*>
      event_registry_;  // TODO: I don't like the naked ptr.
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_EVENT_H_
