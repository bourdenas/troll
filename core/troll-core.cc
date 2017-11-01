#include "core/troll-core.h"

#include "core/world.h"

namespace troll {

Core::FpsCounter Core::fps_counter_;

void Core::Init() { World::Instance().Init(); }

void Core::CleanUp() { World::Instance().UnloadScene(); }

void Core::Run() {
  // World::Instance().LoadScene("main");

  int curr_time = 0;  // TODO: GetTime()
  int prev_time = curr_time;

  while (InputHandling()) {
    // curr_time = SDL_GetTicks();

    FrameStarted(curr_time - prev_time);
    RenderFrame();
    FrameEnded(curr_time - prev_time);

    prev_time = curr_time;
  }
}

bool Core::InputHandling() { return false; }

void Core::RenderFrame() {}

void Core::FrameStarted(int time_since_last_frame) {}

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
