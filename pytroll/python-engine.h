#ifndef TROLL_PYTROLL_PYTHON_ENGINE_H_
#define TROLL_PYTROLL_PYTHON_ENGINE_H_

#include <unordered_map>

#include <pybind11/embed.h>

#include "core/core.h"
#include "core/scripting-engine.h"

namespace troll {

// Python execution engine that handles scripting.
class PythonEngine : public ScriptingEngine {
 public:
  PythonEngine(const std::string& script_base_path, Core* core);
  ~PythonEngine() override;

  // Instantiate a pytroll scene class from specified module.
  void CreateScene(const std::string& scene_id) override;
  void ChangeScene(const pybind11::object& scene);

 private:
  void CreateScene(const std::string& module, const std::string& scene_class);

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

#endif  // TROLL_PYTROLL_PYTHON_ENGINE_H_
