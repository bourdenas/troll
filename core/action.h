#ifndef TROLL_CORE_ACTION_H_
#define TROLL_CORE_ACTION_H_

#include "world-element.h"

namespace troll {

class Action : public WorldElement {
 public:
  Action() = default;
  ~Action() override = default;

  virtual void Exec() = 0;
};

}  // namespace troll

#endif  // TROLL_CORE_ACTION_H_
