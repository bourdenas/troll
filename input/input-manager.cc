#include "input/input-manager.h"

#include <algorithm>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>

#include "action/action-manager.h"

namespace troll {

namespace {
// For an input HOLD trigger return two separate triggers on-press and
// on-release that implement the hold logic while the key is pressed and reverse
// it when it is released.
std::pair<Trigger, Trigger> MakePressReleaseTriggers(
    const Trigger& hold_trigger, const ActionManager* action_manager) {
  Trigger on_press = hold_trigger;
  on_press.set_state(Trigger::PRESSED);

  Trigger on_release;
  on_release.set_state(Trigger::RELEASED);

  // On release trigger all reverse actions of the pressed ignoring irreversible
  // actions.
  const std::vector<Action> on_release_actions =
      hold_trigger.action() |
      ranges::view::transform([action_manager](const Action& action) {
        return action_manager->Reverse(action);
      }) |
      ranges::view::filter(
          [](const Action& reverse) { return !reverse.has_noop(); });
  if (!on_release_actions.empty()) {
    std::copy(on_release_actions.begin(), on_release_actions.end(),
              RepeatedFieldBackInserter(on_release.mutable_action()));
  }

  return std::make_pair(on_press, on_release);
}
}  // namespace

InputManager::InputManager(const KeyBindings& key_bindings,
                           const ActionManager* action_manager)
    : action_manager_(action_manager) {
  for (const auto& context : key_bindings.context()) {
    for (const auto& interaction : context.interaction()) {
      for (const auto& key_combo : interaction.key_combo()) {
        for (const auto& trigger : interaction.trigger()) {
          std::vector<Trigger> triggers = {trigger};
          if (trigger.state() == Trigger::HOLD) {
            const auto trigger_pair =
                MakePressReleaseTriggers(trigger, action_manager_);
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

int InputManager::RegisterHandler(const InputHandler& handler) {
  static int kHandlerId = 0;
  const int handler_id = ++kHandlerId;
  input_handlers_.emplace(handler_id, handler);
  return handler_id;
}

void InputManager::UnregisterHandler(int handler_id) {
  lame_duck_handlers_.push_back(handler_id);
}

void InputManager::ActivateContext(const std::string& context_id) {
  active_contexts_.insert(context_id);
}

void InputManager::DeactivateContext(const std::string& context_id) {
  active_contexts_.erase(context_id);
}

void InputManager::Handle(const InputEvent& event) {
  if (event.has_key_event()) {
    HandleKey(event.key_event());
  }

  // Trigger external input handlers.
  for (const auto& handler : input_handlers_ | ranges::view::values) {
    handler(event);
  }

  if (!lame_duck_handlers_.empty()) {
    for (auto handler_id : lame_duck_handlers_)
      input_handlers_.erase(handler_id);
  }
}

void InputManager::HandleKey(const KeyEvent& event) const {
  for (const auto& context_id : active_contexts_) {
    const auto& key = std::make_pair(event.key(), context_id);
    for (auto it = interactions_.lower_bound(key);
         it != interactions_.upper_bound(key); ++it) {
      const auto& trigger = it->second;
      if (trigger.state() != event.key_state()) continue;

      for (const auto& action : trigger.action()) {
        action_manager_->Execute(action);
      }
    }
  }
}

}  // namespace troll
