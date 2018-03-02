#ifndef TROLL_CORE_UTIL_LIB_H_
#define TROLL_CORE_UTIL_LIB_H_

#include "proto/primitives.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {
namespace util {

Box GetSceneNodeBoundingBox(const SceneNode& node);

double VectorLength(const Vector& v);
double VectorSquaredLength(const Vector& v);

void VectorNormalise(Vector* v);

}  // namespace util

Vector operator+(const Vector& left, const Vector& right);
Vector operator-(const Vector& left, const Vector& right);
Vector operator*(const Vector& v, double a);

}  // namespace troll

#endif  // TROLL_CORE_UTIL_LIB_H_
