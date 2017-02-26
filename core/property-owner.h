#ifndef TROLL_CORE_WORLD_PROPERTY_OWNER_H_
#define TROLL_CORE_WORLD_PROPERTY_OWNER_H_

#include <map>
#include <string>

namespace troll {

class PropertyOwner {
 public:
  PropertyOwner() = default;
  virtual ~PropertyOwner() = default;

 protected:
  std::map<std::string, std::string> properties_;
};

}  // namespace troll

#endif  // TROLL_CORE_WORLD_PROPERTY_OWNER_H_
