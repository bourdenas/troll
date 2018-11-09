#ifndef TROLL_INPUT_INPUT_MANAGER_H_
#define TROLL_INPUT_INPUT_MANAGER_H_

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "input/input-event.h"
#include "proto/key-binding.pb.h"

namespace troll {

class InputManager {
 public:
  static InputManager& Instance() {
    static InputManager singleton;
    return singleton;
  }

  void Init();
  void AddKeyMapping(const std::string& label, int key_code);

  using InputHandler = std::function<void(const InputEvent&)>;
  int RegisterHandler(const InputHandler& handler);
  void UnregisterHandler(int handler_id);

  void ActivateContext(const std::string& context_id);
  void DeactivateContext(const std::string& context_id);

  void Handle(const InputEvent& event) const;

  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

 private:
  InputManager() = default;
  ~InputManager() = default;

  void HandleKey(const InputEvent::KeyEvent& event) const;

  // A mapping from a key code of a input backend into a semantic name.
  std::unordered_map<int, std::string> key_mapping_;

  // Set of activated games input contexts.
  std::unordered_set<std::string> active_contexts_ = {""};

  // Mapping of {key, context} pairs to interaction triggers.
  std::multimap<std::pair<std::string, std::string>, Trigger> interactions_;

  // Registered input handlers.
  std::unordered_map<int, InputHandler> input_handlers_;
};

}  // namespace troll

#endif  // TROLL_INPUT_INPUT_MANAGER_H_
