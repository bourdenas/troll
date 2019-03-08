#ifndef TROLL_CORE_TROLL_CORE_H_
#define TROLL_CORE_TROLL_CORE_H_

#include <memory>
#include <string>

#include "action/action-manager.h"
#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/core.h"
#include "core/event-dispatcher.h"
#include "core/resource-manager.h"
#include "core/scene-manager.h"
#include "core/scripting-engine.h"
#include "input/input-manager.h"
#include "sdl/input-backend.h"
#include "sdl/renderer.h"
#include "sound/audio-mixer.h"
#include "sound/sound-loader.h"

namespace troll {

class TrollCore : public Core {
 public:
  // Takes ownership of ScriptingEngine.
  void Init(const std::string& name, const std::string& resource_base_path,
            ScriptingEngine* engine);

  void Run();
  void Halt() override;
  void LoadScene(const Scene& scene) override;

  ActionManager* action_manager() override { return action_manager_.get(); }
  AnimatorManager* animator_manager() override {
    return animator_manager_.get();
  }
  AudioMixer* audio_mixer() override { return audio_mixer_.get(); }
  CollisionChecker* collision_checker() override {
    return collision_checker_.get();
  }
  EventDispatcher* event_dispatcher() override {
    return event_dispatcher_.get();
  }
  InputBackend* input_backend() override { return input_backend_.get(); }
  InputManager* input_manager() override { return input_manager_.get(); }
  Renderer* renderer() override { return renderer_.get(); }
  ResourceManager* resource_manager() override {
    return resource_manager_.get();
  }
  SceneManager* scene_manager() override { return scene_manager_.get(); }
  ScriptingEngine* scripting_engine() override {
    return scripting_engine_.get();
  }
  SoundLoader* sound_loader() override { return sound_loader_.get(); }

 private:
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
  std::unique_ptr<ScriptingEngine> scripting_engine_;
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
