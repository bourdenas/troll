#include "scripting/script-manager.h"

#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <pybind11/functional.h>

#include "action/action-manager.h"
#include "core/event-dispatcher.h"

namespace troll {

namespace {
// Wraps python callbacks with a try/catch block for displaying run-time errors
// that happen during python execution.
void PythonCallbackWrapper(const std::function<void()>& python_handler) {
  try {
    python_handler();
  } catch (pybind11::error_already_set& e) {
    LOG(ERROR) << "Python run-time error:\n" << e.what();
  }
}
}  // namespace

PYBIND11_EMBEDDED_MODULE(troll, m) {
  m.def("execute", [](const std::string& encoded_action) {
    Action action;
    action.ParseFromString(encoded_action);
    ActionManager::Instance().Execute(action);
  });

  m.def("on_event", [](const std::string& event_id,
                       const std::function<void()>& handler, bool permanent) {
    if (permanent) {
      return EventDispatcher::Instance().RegisterPermanent(
          event_id, [handler]() { PythonCallbackWrapper(handler); });
    } else {
      return EventDispatcher::Instance().Register(
          event_id, [handler]() { PythonCallbackWrapper(handler); });
    }
  });

  m.def("cancel", [](const std::string& event_id, int handler_id) {
    EventDispatcher::Instance().Unregister(event_id, handler_id);
  });
}

void ScriptManager::Init(const std::string& script_base_path) {
  script_base_path_ = script_base_path;

  // Setup python module import paths appropriately.
  sys_module_ = pybind11::module::import("sys");
  for (const auto& path :
       {std::string(".."), absl::StrCat(script_base_path_, "scripts")}) {
    sys_module_.attr("path").attr("append")(path);
  }
}

void ScriptManager::Call(const std::string& module,
                         const std::string& function) {
  const auto* py_module = ImportModule(module);
  try {
    py_module->attr(function.c_str())();
  } catch (const pybind11::error_already_set& e) {
    LOG(ERROR) << "Python run-time error:\n" << e.what();
  }
}

const pybind11::module* ScriptManager::ImportModule(const std::string& module) {
  const auto it = modules_.find(module);
  if (it != modules_.end()) {
    return &it->second;
  }

  try {
    auto&& module_entry =
        modules_.emplace(module, pybind11::module::import(module.c_str()))
            .first;
    return &module_entry->second;
  } catch (const pybind11::error_already_set& e) {
    LOG(FATAL) << "Python import module error:\n" << e.what();
  }
  return nullptr;
}

}  // namespace troll
