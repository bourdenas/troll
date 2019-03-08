#ifndef TROLL_CORE_SCRIPTING_ENGINE_H_
#define TROLL_CORE_SCRIPTING_ENGINE_H_

namespace troll {

// Script execution engine that handles scripting modules.
class ScriptingEngine {
 public:
  ScriptingEngine() = default;
  virtual ~ScriptingEngine() = default;

  // Scripting engines override this function to build the requested scene.
  virtual void CreateScene(const std::string& scene_id) = 0;

  ScriptingEngine(const ScriptingEngine&) = delete;
  ScriptingEngine& operator=(const ScriptingEngine&) = delete;
};

}  // namespace troll

#endif  // TROLL_CORE_SCRIPTING_ENGINE_H_
