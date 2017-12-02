#include "core/troll-core.h"

#include <absl/strings/str_cat.h>

#include "animation/animator-manager.h"
#include "core/action-manager.h"
#include "core/collision-checker.h"
#include "core/resource-manager.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"

namespace troll {

void Core::Init() {
  ActionManager::Instance().Init();

  renderer_ = std::make_unique<Renderer>();
  renderer_->Init(800, 600);

  ResourceManager::Instance().LoadResources(*renderer_);
  LoadScene("main.scene");
}

void Core::CleanUp() {
  ResourceManager::Instance().CleanUp();

  renderer_->CleanUp();
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
  scene_manager_ = std::make_unique<SceneManager>(*renderer_);
  scene_manager_->SetupScene(ResourceManager::Instance().LoadScene(scene_id));
}

bool Core::InputHandling() {
  if (halt_) {
    return false;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return false;
    }
    if (event.type == SDL_KEYDOWN) {
      return false;
    }
  }
  return true;
}

void Core::FrameStarted(int time_since_last_frame) {
  AnimatorManager::Instance().Progress(time_since_last_frame);
  // Physics::Instance().Progress(time_since_last_frame);
  CollisionChecker::Instance().CheckCollisions();
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
