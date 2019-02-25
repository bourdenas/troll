#ifndef TROLL_CORE_CORE_H_
#define TROLL_CORE_CORE_H_

#include "proto/scene.pb.h"

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
  Core() = default;
  virtual ~Core() = default;

  virtual void Halt() = 0;
  virtual void LoadScene(const Scene& scene) = 0;

  virtual ActionManager* action_manager() = 0;
  virtual AnimatorManager* animator_manager() = 0;
  virtual AudioMixer* audio_mixer() = 0;
  virtual CollisionChecker* collision_checker() = 0;
  virtual EventDispatcher* event_dispatcher() = 0;
  virtual InputBackend* input_backend() = 0;
  virtual InputManager* input_manager() = 0;
  virtual Renderer* renderer() = 0;
  virtual ResourceManager* resource_manager() = 0;
  virtual SceneManager* scene_manager() = 0;
  virtual ScriptManager* script_manager() = 0;
  virtual SoundLoader* sound_loader() = 0;

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;
};

}  // namespace troll

#endif  // TROLL_CORE_CORE_H_
