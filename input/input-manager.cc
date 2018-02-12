#include "input/input-manager.h"

#include <algorithm>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"
#include "core/resource-manager.h"

namespace troll {

// For an input HOLD trigger return two separate triggers on-press and
// on-release that implement the hold logic while the key is pressed and reverse
// it when it is released.
std::pair<Trigger, Trigger> MakePressReleaseTriggers(
    const Trigger& hold_trigger) {
  Trigger on_press = hold_trigger;
  on_press.set_state(Trigger::PRESSED);

  Trigger on_release;
  on_release.set_state(Trigger::RELEASED);

  // On release trigger all reverse actions of the pressed ignoring irreversible
  // actions.
  const std::vector<Action> on_release_actions =
      hold_trigger.action() | ranges::view::transform([](const Action& action) {
        return ActionManager::Instance().Reverse(action);
      }) |
      ranges::view::filter(
          [](const Action& reverse) { return !reverse.has_noop(); });
  if (!on_release_actions.empty()) {
    std::copy(on_release_actions.begin(), on_release_actions.end(),
              RepeatedFieldBackInserter(on_release.mutable_action()));
  }

  return std::make_pair(on_press, on_release);
}

void InputManager::Init() {
  const auto& bindings = ResourceManager::Instance().GetKeyBindings();
  for (const auto& context : bindings.context()) {
    for (const auto& interaction : context.interaction()) {
      for (const auto& key_combo : interaction.key_combo()) {
        for (const auto& trigger : interaction.trigger()) {
          std::vector<Trigger> triggers = {trigger};
          if (trigger.state() == Trigger::HOLD) {
            const auto trigger_pair = MakePressReleaseTriggers(trigger);
            triggers = {std::get<0>(trigger_pair), std::get<1>(trigger_pair)};
          }
          for (const auto& trigger : triggers) {
            interactions_.emplace(
                std::make_pair(key_combo.key_code(0), context.id()), trigger);
          }
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
