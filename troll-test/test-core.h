#ifndef TROLL_TEST_TROLL_TEST_CORE_H_
#define TROLL_TEST_TROLL_TEST_CORE_H_

#include "core/core.h"

namespace troll {

class TestCore : public Core {
 public:
  void Halt() override {}
  void LoadScene(const Scene& scene) override {}

  ActionManager* action_manager() override { return action_manager_; }
  AnimatorManager* animator_manager() override { return animator_manager_; }
  AudioMixer* audio_mixer() override { return audio_mixer_; }
  CollisionChecker* collision_checker() override { return collision_checker_; }
  EventDispatcher* event_dispatcher() override { return event_dispatcher_; }
  InputBackend* input_backend() override { return input_backend_; }
  InputManager* input_manager() override { return input_manager_; }
  Renderer* renderer() override { return renderer_; }
  ResourceManager* resource_manager() override { return resource_manager_; }
  SceneManager* scene_manager() override { return scene_manager_; }
  ScriptingEngine* scripting_engine() override { return scripting_engine_; }
  SoundLoader* sound_loader() override { return sound_loader_; }

  void set_action_manager(ActionManager* action_manager) {
    action_manager_ = action_manager;
  }
  void set_animator_manager(AnimatorManager* animator_manager) {
    animator_manager_ = animator_manager;
  }
  void set_audio_mixer(AudioMixer* audio_mixer) { audio_mixer_ = audio_mixer; }
  void set_collision_checker(CollisionChecker* collision_checker) {
    collision_checker_ = collision_checker;
  }
  void set_event_dispatcher(EventDispatcher* event_dispatcher) {
    event_dispatcher_ = event_dispatcher;
  }
  void set_input_backend(InputBackend* input_backend) {
    input_backend_ = input_backend;
  }
  void set_input_manager(InputManager* input_manager) {
    input_manager_ = input_manager;
  }
  void set_renderer(Renderer* renderer) { renderer_ = renderer; }
  void set_resource_manager(ResourceManager* resource_manager) {
    resource_manager_ = resource_manager;
  }
  void set_scene_manager(SceneManager* scene_manager) {
    scene_manager_ = scene_manager;
  }
  void set_scripting_engine(ScriptingEngine* scripting_engine) {
    scripting_engine_ = scripting_engine;
  }
  void set_sound_loader(SoundLoader* sound_loader) {
    sound_loader_ = sound_loader;
  }

 private:
  ActionManager* action_manager_ = nullptr;
  AnimatorManager* animator_manager_ = nullptr;
  AudioMixer* audio_mixer_ = nullptr;
  CollisionChecker* collision_checker_ = nullptr;
  EventDispatcher* event_dispatcher_ = nullptr;
  InputBackend* input_backend_ = nullptr;
  InputManager* input_manager_ = nullptr;
  Renderer* renderer_ = nullptr;
  ResourceManager* resource_manager_ = nullptr;
  SceneManager* scene_manager_ = nullptr;
  ScriptingEngine* scripting_engine_ = nullptr;
  SoundLoader* sound_loader_ = nullptr;
};

}  // namespace troll

#endif  // TROLL_TEST_TROLL_TEST_CORE_H_
