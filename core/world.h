#ifndef TROLL_CORE_WORLD_H_
#define TROLL_CORE_WORLD_H_

#include <memory>
#include <string>
#include <unordered_map>

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

  using Characters =
      std::unordered_map<std::string, std::unique_ptr<Character>>;
  using Objects = std::unordered_map<std::string, std::unique_ptr<Object>>;

  Characters& characters() { return characters_; }
  Objects& objects() { return objects_; }
  Scene& scene() { return *scene_; }

  World(const World&) = delete;
  World& operator=(const World&) = delete;

 private:
  World() = default;
  ~World() = default;

  void LoadSprites();

  Characters characters_;
  Objects objects_;
  std::unique_ptr<Scene> scene_;

  std::unique_ptr<SceneManager> scene_manager_;
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_H_
