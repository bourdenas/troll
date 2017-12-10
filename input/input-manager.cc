#include "input/input-manager.h"

#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>

#include "core/action-manager.h"
#include "core/resource-manager.h"

namespace troll {

void InputManager::Init() {
  const auto& bindings = ResourceManager::Instance().GetKeyBindings();
  for (const auto& context : bindings.context()) {
    for (const auto& interaction : context.interaction()) {
      for (const auto& key_combo : interaction.key_combo()) {
        for (const auto& trigger : interaction.trigger()) {
          interactions_.emplace(
              std::make_pair(key_combo.key_code(0), context.id()), trigger);
        }
      }
    }
  }
}

void InputManager::AddKeyMapping(const std::string& label, int key_code) {
  key_mapping_.emplace(key_code, label);
}

void InputManager::ActivateContext(const std::string& context_id) {
  active_contexts_.insert(context_id);
}

void InputManager::DeactivateContext(const std::string& context_id) {
  active_contexts_.erase(context_id);
}

void InputManager::Handle(const InputEvent& event) const {
  if (event.event_type() == InputEvent::EventType::KEY_EVENT) {
    HandleKey(event.key_event());
  }
}

void InputManager::HandleKey(const InputEvent::KeyEvent& event) const {
  const auto it = key_mapping_.find(event.key_code);
  if (it == key_mapping_.end()) return;

  for (const auto& context_id : active_contexts_) {
    const auto& key = std::make_pair(it->second, context_id);
    for (auto iit = interactions_.lower_bound(key);
         iit != interactions_.upper_bound(key); ++iit) {
      const auto& trigger = iit->second;
      if (trigger.state() != event.key_state) continue;

      for (const auto& action : trigger.action()) {
        ActionManager::Instance().Execute(action);
      }
    }
  }
}

}  // namespace troll
