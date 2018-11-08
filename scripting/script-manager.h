#ifndef TROLL_SCRIPTING_SCRIPT_MANAGER_H_
#define TROLL_SCRIPTING_SCRIPT_MANAGER_H_

#include <memory>
#include <unordered_map>

#include <pybind11/embed.h>

namespace troll {

// Script execution engine that handles scripting modules and evaluates
// expressions.
class ScriptManager {
 public:
  static ScriptManager& Instance() {
    static ScriptManager singleton;
    return singleton;
  }

  void Init(const std::string& script_base_path);

  // Call an imported module's function in the scripting engine.
  void Call(const std::string& module, const std::string& function);

  ScriptManager(const ScriptManager&) = delete;
  ScriptManager& operator=(const ScriptManager&) = delete;

 private:
  // Import a module that is in the import path.
  const pybind11::module* ImportModule(const std::string& module);

  ScriptManager() = default;
  ~ScriptManager() = default;

  std::string script_base_path_;

  pybind11::scoped_interpreter intpt_;
  pybind11::module sys_module_;

  std::unordered_map<std::string, pybind11::module> modules_;
};

}  // namespace troll

#endif  // TROLL_SCRIPTING_SCRIPT_MANAGER_H_
