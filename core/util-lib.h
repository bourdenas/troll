#ifndef TROLL_CORE_UTIL_LIB_H_
#define TROLL_CORE_UTIL_LIB_H_

#include "proto/primitives.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {
namespace util {

Box GetSceneNodeBoundingBox(const SceneNode& node);

}  // namespace util
}  // namespace troll

#endif  // TROLL_CORE_UTIL_LIB_H_
