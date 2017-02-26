#ifndef TROLL_CORE_WORLD_ELEMENT_H_
#define TROLL_CORE_WORLD_ELEMENT_H_

#include <map>
#include <memory>
#include <string>

#include "../build/sprite.pb.h"
#include "property-owner.h"

namespace troll {

// Base construct of the game world. Every game element is derived from this
// class.
class WorldElement : public PropertyOwner {
 public:
  WorldElement(const std::string& id) : id_(id) {}
  virtual ~WorldElement() = default;

  const std::string& id() const { return id_; }

 private:
  std::string id_;
};

// An object prototype of the game world.
class Object : public WorldElement {
 public:
  Object(const std::string& id) : WorldElement(id) {}
  ~Object() override = default;

 private:
  Sprite sprite_;
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_ELEMENT_H_
