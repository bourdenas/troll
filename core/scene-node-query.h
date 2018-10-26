#ifndef TROLL_CORE_SCENE_NODE_QUERY_H_
#define TROLL_CORE_SCENE_NODE_QUERY_H_

#include <string>

#include "proto/scene-node.pb.h"

namespace troll {

struct SceneNodeQuery {
  bool Parse(const std::string& pattern);

  enum class RetrievalMode {
    INVALID,
    GLOBAL,
    LOCAL,
  };

  RetrievalMode mode;
  SceneNode pattern;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_NODE_QUERY_H_
