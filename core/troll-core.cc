#include "core/troll-core.h"

#include "action/action-manager.h"
#include "animation/animator-manager.h"
#include "core/collision-checker.h"
#include "core/event-dispatcher.h"
#include "core/resource-manager.h"
#include "input/input-manager.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "scripting/script-manager.h"
#include "sdl/input-backend.h"
#include "sdl/renderer.h"
#include "sound/audio-mixer.h"
#include "sound/sound-loader.h"

namespace troll {

void Core::Init() {
  ActionManager::Instance().Init();

  Renderer::Instance().Init(640, 480);
  SoundLoader::Instance().Init();
  AudioMixer::Instance().Init();

  ResourceManager::Instance().LoadResources();
  LoadScene("main.scene");

  InputManager::Instance().Init();
  InputBackend::Instance().Init();
  ScriptManager::Instance().Init();
}

void Core::CleanUp() {
  ResourceManager::Instance().CleanUp();
  SoundLoader::Instance().CleanUp();
  Renderer::Instance().CleanUp();
}

void Core::Run() {
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

void Core::Halt() { halt_ = true; }

void Core::LoadScene(const std::string& scene_id) {
  EventDispatcher::Instance().UnregisterAll();
  scene_manager_ = std::make_unique<SceneManager>();
  scene_manager_->SetupScene(ResourceManager::Instance().LoadScene(scene_id));
}

bool Core::InputHandling() {
  if (halt_) {
    return false;
  }

  InputEvent event;
  while ((event = InputBackend::Instance().PollEvent()).event_type() !=
         InputEvent::EventType::NO_EVENT) {
    if (event.event_type() == InputEvent::EventType::QUIT_EVENT) {
      return false;
    }
    InputManager::Instance().Handle(event);
  }
  return true;
}

void Core::FrameStarted(int time_since_last_frame) {
  AnimatorManager::Instance().Progress(time_since_last_frame);
  CollisionChecker::Instance().CheckCollisions();
  EventDispatcher::Instance().ProcessTriggeredEvents();
}

void Core::FrameEnded(int time_since_last_frame) {
  ++fps_counter_.fp_count;
  fps_counter_.elapsed_time += time_since_last_frame;

  if (fps_counter_.elapsed_time < 1000) {
    return;
  }
  fps_counter_.fps = fps_counter_.fp_count;
  fps_counter_.elapsed_time = fps_counter_.fp_count = 0;
}

}  // namespace troll
