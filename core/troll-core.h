#ifndef TROLL_CORE_TROLL_CORE_H_
#define TROLL_CORE_TROLL_CORE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "core/scene-manager.h"
#include "core/scene.h"
#include "core/world-element.h"
#include "sdl/renderer.h"

namespace troll {

class Core {
 public:
  static Core& Instance() {
    static Core singleton;
    return singleton;
  }

  void Init();
  void CleanUp();
  void Run();

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;

 private:
  Core() = default;
  virtual ~Core() = default;

  bool InputHandling();

  void FrameStarted(int time_since_last_frame);
  void RenderFrame();
  void FrameEnded(int time_since_last_frame);

  void LoadScene(const std::string& scene);
  void UnloadScene();
  void LoadSprites();

  std::unordered_map<std::string, std::unique_ptr<Character>> characters_;
  std::unordered_map<std::string, std::unique_ptr<Object>> objects_;
  std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;

  std::unique_ptr<SceneManager> scene_manager_;

  struct FpsCounter {
    int elapsed_time = 0;
    int fps = 0;
    int fp_count = 0;
  };
  FpsCounter fps_counter_;

  std::unique_ptr<Renderer> renderer_;
};

}  // namespace troll

#endif  // TROLL_CORE_TROLL_CORE_H_
