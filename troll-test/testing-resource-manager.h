#ifndef TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_
#define TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_

#include "core/resource-manager.h"
#include "proto/sprite.pb.h"

namespace troll {

class TestingResourceManager {
 public:
  static void SetTestSprite(const Sprite& sprite) {
    ResourceManager::Instance().sprites_[sprite.id()] = sprite;
  }

  static void SetTestAnimationScript(const AnimationScript& script) {
    ResourceManager::Instance().scripts_[script.id()] = script;
  }
};

}  // namespace troll

#endif  // TROLL_TROLL_TEST_TESTING_RESOURCE_MANAGER_H_
