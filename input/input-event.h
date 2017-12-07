#ifndef TROLL_INPUT_INPUT_EVENT_H_
#define TROLL_INPUT_INPUT_EVENT_H_

#include "proto/key-binding.pb.h"

namespace troll {

class InputEvent {
 public:
  enum class EventType {
    NO_EVENT,
    QUIT_EVENT,
    KEY_EVENT,
    MOUSE_EVENT,
  };

  struct QuitEvent {};

  struct KeyEvent {
    int key_code;
    int key_modifiers;
    Trigger::KeyState key_state;
  };

  struct MouseEvent {
    int button_code;
    Trigger::KeyState key_state;

    struct Point {
      int x, y;
    };
    Point absolute_position;
    Point relative_position;
  };

  InputEvent() : event_type_(EventType::NO_EVENT) {}
  InputEvent(const QuitEvent& quit_event)
      : event_type_(EventType::QUIT_EVENT) {}
  InputEvent(const KeyEvent& key_event) : event_type_(EventType::KEY_EVENT) {
    event_.key_event_ = key_event;
  }
  InputEvent(const MouseEvent& mouse_event)
      : event_type_(EventType::MOUSE_EVENT) {
    event_.mouse_event_ = mouse_event;
  }

  EventType event_type() { return event_type_; }

  const KeyEvent& key_event() const { return event_.key_event_; }
  const MouseEvent& mouse_event() const { return event_.mouse_event_; }

 private:
  EventType event_type_;

  union EventUnion {
    KeyEvent key_event_;
    MouseEvent mouse_event_;
  } event_;
};

}  // namespace troll

#endif  // TROLL_INPUT_INPUT_EVENT_H_
