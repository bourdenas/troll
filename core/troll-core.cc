#include "core/troll-core.h"

#include "core/world.h"
#include "sdl/renderer.h"

namespace troll {

Core::FpsCounter Core::fps_counter_;
std::unique_ptr<Renderer> Core::renderer_;

void Core::Init() {
  World::Instance().Init();
  renderer_ = std::make_unique<Renderer>();
  renderer_->Init();
}

void Core::CleanUp() {
  renderer_->CleanUp();
  World::Instance().UnloadScene();
}

void Core::Run() {
  int curr_time = 0;  // SDL_GetTicks();
  int prev_time = curr_time;

  while (InputHandling()) {
    // curr_time = SDL_GetTicks();

    FrameStarted(curr_time - prev_time);
    RenderFrame();
    FrameEnded(curr_time - prev_time);
    SDL_Delay(1000);

    prev_time = curr_time;
  }
}

bool Core::InputHandling() {
  static int i = 0;
  return i++ < 3;
}

void Core::RenderFrame() {
  auto texture = renderer_->LoadTexture("../data/resources/hello.bmp");
  renderer_->ClearScreen();
  renderer_->BlitTexture(*texture, Box(), Box());
  renderer_->Flip();
}

void Core::FrameStarted(int time_since_last_frame) {
  // AnimatorManager::Instance().Progress(timeSinceLastFrame);
  // Physics::Instance().Progress(timeSinceLastFrame);
  // CollisionChecker::Instance().PerformCollisions();
}

void Core::FrameEnded(int time_since_last_frame) {
  //-- Destroy LatelyDestroyable objects
  // DestructionManager::Commit();

  ++fps_counter_.fp_count;
  fps_counter_.elapsed_time += time_since_last_frame;

  if (fps_counter_.elapsed_time < 1000) {
    return;
  }
  fps_counter_.fps = fps_counter_.fp_count;
  fps_counter_.elapsed_time = fps_counter_.fp_count = 0;
}

}  // namespace troll
