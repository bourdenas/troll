#include "core/util-lib.h"

#include <math.h>

#include "core/resource-manager.h"
#include "proto/sprite.pb.h"

namespace troll {
namespace util {

double VectorLength(const Vector& v) {
  return std::sqrt(VectorSquaredLength(v));
}

double VectorSquaredLength(const Vector& v) {
  return v.x() * v.x() + v.y() * v.y() + v.z() * v.z();
}

void VectorNormalise(Vector* v) {
  const auto len = VectorLength(*v);
  if (len > 1e-08) {
    *v = *v * (1.0f / len);
  }
}

}  // namespace util

Vector operator+(const Vector& left, const Vector& right) {
  Vector v;
  v.set_x(left.x() + right.x());
  v.set_y(left.y() + right.y());
  v.set_z(left.z() + right.z());
  return v;
}

Vector operator-(const Vector& left, const Vector& right) {
  Vector v;
  v.set_x(left.x() - right.x());
  v.set_y(left.y() - right.y());
  v.set_z(left.z() - right.z());
  return v;
}

Vector operator*(const Vector& v, double a) {
  Vector u;
  u.set_x(v.x() * a);
  u.set_y(v.y() * a);
  u.set_z(v.z() * a);
  return u;
}

}  // namespace troll
