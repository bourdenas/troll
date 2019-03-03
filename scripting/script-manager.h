#ifndef TROLL_SCRIPTING_SCRIPT_MANAGER_H_
#define TROLL_SCRIPTING_SCRIPT_MANAGER_H_

#include <unordered_map>

#include <pybind11/embed.h>

#include "core/core.h"

namespace troll {

// Script execution engine that handles scripting modules.
class ScriptManager {
 public:
  ScriptManager(const std::string& script_base_path, Core* core);
  ~ScriptManager();

  // Instantiate a pytroll scene class from specified module.
  void CreateScene(const std::string& module, const std::string& scene_class);
  void ChangeScene(const pybind11::object& scene);

  ScriptManager(const ScriptManager&) = delete;
  ScriptManager& operator=(const ScriptManager&) = delete;

 private:
  // Import a module that can be found in the import path.
  const pybind11::module* ImportModule(const std::string& module);

  // The python object that contains the current game scene.
  pybind11::object scene_;

  std::string script_base_path_;

  pybind11::scoped_interpreter intpt_;
  pybind11::module sys_module_;

  std::unordered_map<std::string, pybind11::module> modules_;
};

}  // namespace troll

#endif  // TROLL_SCRIPTING_SCRIPT_MANAGER_H_
