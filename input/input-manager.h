#ifndef TROLL_INPUT_INPUT_MANAGER_H_
#define TROLL_INPUT_INPUT_MANAGER_H_

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "action/action-manager.h"
#include "proto/input-event.pb.h"
#include "proto/key-binding.pb.h"

namespace troll {

class InputManager {
 public:
  InputManager(const KeyBindings& key_bindings,
               const ActionManager* action_manager);
  ~InputManager() = default;

  using InputHandler = std::function<void(const InputEvent&)>;
  int RegisterHandler(const InputHandler& handler);
  void UnregisterHandler(int handler_id);

  void ActivateContext(const std::string& context_id);
  void DeactivateContext(const std::string& context_id);

  void Handle(const InputEvent& event);

  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

 private:
  void HandleKey(const KeyEvent& event) const;

  const ActionManager* action_manager_;

  // Set of activated games input contexts.
  std::unordered_set<std::string> active_contexts_ = {""};

  // Mapping of {key, context} pairs to interaction triggers.
  std::multimap<std::pair<std::string, std::string>, Trigger> interactions_;

  // Registered input handlers.
  std::unordered_map<int, InputHandler> input_handlers_;

  // Handler ids that should be removed. Removal is delayed because it may
  // happen during handling loop.
  std::vector<int> lame_duck_handlers_;
};

}  // namespace troll

#endif  // TROLL_INPUT_INPUT_MANAGER_H_
