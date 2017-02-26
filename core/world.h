#ifndef TROLL_CORE_WORLD_H_
#define TROLL_CORE_WORLD_H_

#include <map>
#include <memory>
#include <string>

#include "scene-manager.h"
#include "scene.h"
#include "world-element.h"

namespace troll {

// Central object of the game world.
class World {
 public:
  static World& Instance() {
    static World singleton;
    return singleton;
  }

  void Init();
  void Run();

  void LoadScene(const std::string& scene);
  void UnloadScene();

  using Characters = std::map<std::string, std::unique_ptr<Character>>;
  using Objects = std::map<std::string, std::unique_ptr<Object>>;
  using Scenes = std::map<std::string, std::unique_ptr<Scene>>;

  Characters& characters() { return characters_; }
  Objects& objects() { return objects_; }
  Scenes& scenes() { return scenes_; }

  World(const World&) = delete;
  World& operator=(const World&) = delete;

 private:
  World() = default;
  ~World() = default;

  Characters characters_;
  Objects objects_;
  Scenes scenes_;

  std::unique_ptr<SceneManager> scene_manager_;
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_H_
