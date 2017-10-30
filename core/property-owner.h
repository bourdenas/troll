#ifndef TROLL_CORE_WORLD_PROPERTY_OWNER_H_
#define TROLL_CORE_WORLD_PROPERTY_OWNER_H_

#include <string>
#include <unordered_map>

namespace troll {

class PropertyOwner {
 public:
  PropertyOwner() = default;
  virtual ~PropertyOwner() = default;

 protected:
  std::unordered_map<std::string, std::string> properties_;
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_PROPERTY_OWNER_H_
