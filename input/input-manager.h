#ifndef TROLL_INPUT_INPUT_MANAGER_H_
#define TROLL_INPUT_INPUT_MANAGER_H_

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

  void ActivateContext(const std::string& context_id);
  void DeactivateContext(const std::string& context_id);

  void Handle(const InputEvent& event);

  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

 private:
  InputManager() = default;
  ~InputManager() = default;

  std::unordered_map<int, std::string> key_mapping_;
  std::unordered_set<std::string> active_contexts_ = {""};
  std::multimap<std::string, Trigger> interactions_;
};

}  // namespace troll

#endif  // TROLL_INPUT_INPUT_MANAGER_H_
