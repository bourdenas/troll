#include "scripting/script-manager.h"

#include <glog/logging.h>

#include "action/action-manager.h"

namespace troll {

PYBIND11_EMBEDDED_MODULE(troll, m) {
  m.def("execute", [](const std::string& encoded_action) {
    Action action;
    action.ParseFromString(encoded_action);
    ActionManager::Instance().Execute(action);
  });
}

void ScriptManager::Init() {}

void ScriptManager::ImportModule(const std::string& module) {
  // Setup python module import paths appropriately.
  auto sys = pybind11::module::import("sys");
  for (const auto& path : {"..", "../data/scripts"}) {
    sys.attr("path").attr("append")(path);
  }

  try {
    modules_.emplace(module, pybind11::module::import(module.c_str()));
  } catch (const pybind11::error_already_set& e) {
    LOG(ERROR) << e.what();
  }
}

void ScriptManager::Call(const std::string& module,
                         const std::string& function) const {
  const auto it = modules_.find(module);
  DLOG_IF(FATAL, it == modules_.end())
      << "Using module '" << module << "' that was not imported first.\nCall: '"
      << module << "." << function << "()'";

  try {
    it->second.attr(function.c_str())();
  } catch (const pybind11::error_already_set& e) {
    LOG(ERROR) << e.what();
  }
}

}  // namespace troll
