#include "pytroll/python-engine.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>
#include <glog/logging.h>
#include <pybind11/functional.h>

#include "action/action-manager.h"
#include "core/event-dispatcher.h"
#include "input/input-manager.h"
#include "proto/action.pb.h"
#include "proto/input-event.pb.h"

namespace troll {

namespace {
// TODO(bourdenas): Figure out a less hacky way to pass the Core instance to the
// embedded python module.
Core* core_instance;

// Returns an EventHandler that wraps a python handler for events. The wrapper
// handles marshaling of events that are passed to python encoded as strings.
EventHandler PythonEventHandlerWrapper(
    const std::function<void(const pybind11::bytes&)>& python_handler) {
  return [python_handler](const Event& event) {
    std::string encoded_event;
    event.SerializeToString(&encoded_event);

    try {
      python_handler(pybind11::bytes(encoded_event));
    } catch (pybind11::error_already_set& e) {
      LOG(ERROR) << "Python run-time error:\n" << e.what();
    }
  };
}

// Returns an InputHandler that wraps a python handler for input events. The
// wrapper handles marshaling of input events that are passed to python encoded
// as strings.
InputManager::InputHandler PythonInputHandlerWrapper(
    const std::function<void(const pybind11::bytes&)>& python_handler) {
  return [python_handler](const InputEvent& event) {
    std::string encoded_input_event;
    event.SerializeToString(&encoded_input_event);

    try {
      python_handler(pybind11::bytes(encoded_input_event));
    } catch (pybind11::error_already_set& e) {
      LOG(ERROR) << "Python run-time error:\n" << e.what();
    }
  };
}
}  // namespace

PYBIND11_EMBEDDED_MODULE(troll, m) {
  m.def("execute", [](const std::string& encoded_action) {
    Action action;
    action.ParseFromString(encoded_action);
    core_instance->action_manager()->Execute(action);
  });

  m.def("transition_scene", [](const pybind11::object& scene) {
    static_cast<PythonEngine*>(core_instance->scripting_engine())
        ->ChangeScene(scene);
  });

  m.def("register_event_handler",
        [](const std::string& event_id,
           const std::function<void(const pybind11::bytes&)>& handler,
           bool permanent) {
          if (permanent) {
            return core_instance->event_dispatcher()->RegisterPermanent(
                event_id, PythonEventHandlerWrapper(handler));
          } else {
            return core_instance->event_dispatcher()->Register(
                event_id, PythonEventHandlerWrapper(handler));
          }
        });

  m.def("cancel_event_handler",
        [](const std::string& event_id, int handler_id) {
          core_instance->event_dispatcher()->Unregister(event_id, handler_id);
        });

  m.def("register_input_handler",
        [](const std::function<void(const pybind11::bytes&)>& handler) {
          return core_instance->input_manager()->RegisterHandler(
              PythonInputHandlerWrapper(handler));
        });

  m.def("cancel_input_handler", [](int handler_id) {
    core_instance->input_manager()->UnregisterHandler(handler_id);
  });
}

PythonEngine::PythonEngine(const std::string& script_base_path, Core* core) {
  core_instance = core;
  script_base_path_ = script_base_path;

  // Setup python module import paths appropriately.
  sys_module_ = pybind11::module::import("sys");
  for (const auto& path : {
           std::string(".."),
           std::string("../proto"),
           absl::StrCat(script_base_path_, "scripts"),
       }) {
    sys_module_.attr("path").attr("append")(path);
  }
}

PythonEngine::~PythonEngine() { core_instance = nullptr; }

void PythonEngine::CreateScene(const std::string& scene_id) {
  // Expeted format is FQN of Python scene class, e.g. "dk.intro.IntroScene".
  std::vector<std::string> tokens = absl::StrSplit(scene_id, '.');
  CreateScene(absl::StrJoin(tokens.begin(), tokens.end() - 1, "."),
              tokens.back());
}

void PythonEngine::CreateScene(const std::string& module,
                               const std::string& scene_class) {
  const auto* py_module = ImportModule(module);
  try {
    auto&& scene = py_module->attr(scene_class.c_str())();
    ChangeScene(scene);
  } catch (const pybind11::error_already_set& e) {
    LOG(ERROR) << "Python run-time error:\n" << e.what();
  }
}

void PythonEngine::ChangeScene(const pybind11::object& scene) {
  if (scene_) {
    scene_.attr("Cleanup")();
  }
  scene_ = scene;
  scene_.attr("Build")();
  scene_.attr("Setup")();
}

const pybind11::module* PythonEngine::ImportModule(const std::string& module) {
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
