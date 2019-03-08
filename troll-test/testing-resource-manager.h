#ifndef TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_
#define TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_

#include "core/resource-manager.h"
#include "proto/sprite.pb.h"

namespace troll {

class TestingResourceManager {
 public:
  TestingResourceManager(ResourceManager* resource_manager)
      : resource_manager_(resource_manager) {}

  void SetTestSprite(const Sprite& sprite) {
    resource_manager_->sprites_[sprite.id()] = sprite;
    auto& masks = resource_manager_->sprite_collision_masks_[sprite.id()];
    for (const auto& film : sprite.film()) {
      masks.push_back(std::vector<bool>(film.width() * film.height(), true));
    }
  }

  void SetTestAnimationScript(const AnimationScript& script) {
    resource_manager_->scripts_[script.id()] = script;
  }

 private:
  ResourceManager* resource_manager_;
};

}  // namespace troll

#endif  // TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_
