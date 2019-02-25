#ifndef TROLL_CORE_TROLL_CORE_H_
#define TROLL_CORE_TROLL_CORE_H_

#include <memory>
#include <string>

#include "action/action-manager.h"
#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/event-dispatcher.h"
#include "core/resource-manager.h"
#include "core/scene-manager.h"
#include "input/input-manager.h"
#include "scripting/script-manager.h"
#include "sdl/input-backend.h"
#include "sdl/renderer.h"
#include "sound/audio-mixer.h"
#include "sound/sound-loader.h"

namespace troll {

class ActionManager;
class AnimatorManager;
class AudioMixer;
class CollisionChecker;
class EventDispatcher;
class InputBackend;
class InputManager;
class Renderer;
class ResourceManager;
class SceneManager;
class ScriptManager;
class SoundLoader;

class Core {
 public:
  static Core& Instance() {
    static Core singleton;
    return singleton;
  }

  void Init(const std::string& name, const std::string& resource_base_path);
  void CleanUp();

  void Run();
  void Halt();

  void LoadScene(const Scene& scene);

  ActionManager& action_manager() { return *action_manager_; }
  AnimatorManager& animator_manager() { return *animator_manager_; }
  AudioMixer& audio_mixer() { return *audio_mixer_; }
  CollisionChecker& collision_checker() { return *collision_checker_; }
  EventDispatcher& event_dispatcher() { return *event_dispatcher_; }
  InputBackend& input_backend() { return *input_backend_; }
  InputManager& input_manager() { return *input_manager_; }
  Renderer& renderer() { return *renderer_; }
  ResourceManager& resource_manager() { return *resource_manager_; }
  SceneManager& scene_manager() { return *scene_manager_; }
  ScriptManager& script_manager() { return *script_manager_; }
  SoundLoader& sound_loader() { return *sound_loader_; }

  // NB: Intended to be used only as dependency injection for unit-tests.
  // Takes ownership.
  void SetupTestSceneManager(SceneManager* scene_manager) {
    scene_manager_.reset(scene_manager);
  }

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;

 private:
  Core() = default;
  ~Core() = default;

  bool InputHandling();

  void FrameStarted(int time_since_last_frame);
  void FrameEnded(int time_since_last_frame);

  std::unique_ptr<ActionManager> action_manager_;
  std::unique_ptr<AnimatorManager> animator_manager_;
  std::unique_ptr<AudioMixer> audio_mixer_;
  std::unique_ptr<CollisionChecker> collision_checker_;
  std::unique_ptr<EventDispatcher> event_dispatcher_;
  std::unique_ptr<InputBackend> input_backend_;
  std::unique_ptr<InputManager> input_manager_;
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<ResourceManager> resource_manager_;
  std::unique_ptr<SceneManager> scene_manager_;
  std::unique_ptr<ScriptManager> script_manager_;
  std::unique_ptr<SoundLoader> sound_loader_;

  struct FpsCounter {
    int elapsed_time = 0;
    int fps = 0;
    int fp_count = 0;
  };
  FpsCounter fps_counter_;

  bool halt_ = false;
};

}  // namespace troll

#endif  // TROLL_CORE_TROLL_CORE_H_
