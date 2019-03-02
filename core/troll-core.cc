#include "core/troll-core.h"

#include <absl/strings/str_cat.h>
#include <glog/logging.h>

#include "proto/input-event.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"

namespace troll {

void TrollCore::Init(const std::string& name,
                     const std::string& resource_base_path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  google::InitGoogleLogging(name.c_str());

  renderer_ = std::make_unique<Renderer>();
  renderer_->CreateWindow(640, 480);

  sound_loader_ = std::make_unique<SoundLoader>();
  sound_loader_->Init();

  resource_manager_ = std::make_unique<ResourceManager>();
  resource_manager_->LoadResources(resource_base_path, renderer_.get(),
                                   sound_loader_.get());

  audio_mixer_ = std::make_unique<AudioMixer>(resource_manager_.get());
  input_backend_ = std::make_unique<InputBackend>();
  script_manager_ = std::make_unique<ScriptManager>(resource_base_path, this);

  action_manager_ = std::make_unique<ActionManager>(this);
  input_manager_ = std::make_unique<InputManager>(
      resource_manager_->GetKeyBindings(), action_manager_.get());

  LoadScene(resource_manager_->LoadScene(
      absl::StrCat(resource_base_path, "scenes/main.scene")));
}

void TrollCore::Run() {
  int curr_time = SDL_GetTicks();
  int prev_time = curr_time;

  while (InputHandling()) {
    curr_time = SDL_GetTicks();

    FrameStarted(curr_time - prev_time);
    scene_manager_->Render();
    FrameEnded(curr_time - prev_time);

    prev_time = curr_time;
  }
}

void TrollCore::Halt() { halt_ = true; }

void TrollCore::LoadScene(const Scene& scene) {
  scene_manager_ = std::make_unique<SceneManager>(resource_manager_.get(),
                                                  renderer_.get(), this);
  animator_manager_ = std::make_unique<AnimatorManager>(this);
  collision_checker_ = std::make_unique<CollisionChecker>(
      scene_manager_.get(), action_manager_.get(), this);
  event_dispatcher_ = std::make_unique<EventDispatcher>();

  scene_manager_->SetupScene(scene, script_manager_.get());
}

bool TrollCore::InputHandling() {
  if (halt_) {
    return false;
  }

  InputEvent event;
  while (!(event = input_backend_->PollEvent()).has_no_event()) {
    if (event.has_quit_event()) {
      return false;
    }
    input_manager_->Handle(event);
  }
  return true;
}

void TrollCore::FrameStarted(int time_since_last_frame) {
  animator_manager_->Progress(time_since_last_frame);
  collision_checker_->CheckCollisions();
  event_dispatcher_->ProcessTriggeredEvents();
}

void TrollCore::FrameEnded(int time_since_last_frame) {
  ++fps_counter_.fp_count;
  fps_counter_.elapsed_time += time_since_last_frame;

  if (fps_counter_.elapsed_time < 1000) {
    return;
  }
  fps_counter_.fps = fps_counter_.fp_count;
  fps_counter_.elapsed_time = fps_counter_.fp_count = 0;
}

}  // namespace troll
