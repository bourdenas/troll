#include "core/geometry.h"

#include <math.h>

namespace troll {
namespace geo {

bool Contains(const Box& box, const Vector& v) {
  return !((v.x() < box.left()) || (v.x() >= box.left() + box.width()) ||
           (v.y() <= box.top()) || (v.y() > box.top() + box.height()));
}

bool Collide(const Box& lhs, const Box& rhs) {
  // Derived from:
  // abs(lhs.x - rhs.x) * 2 < lhs.w + rhs.w &&
  // abs(lhs.y - rhs.y) * 2 < lhs.h + rhs.h
  // where (x,y) is box's centre point.
  return abs(2 * lhs.left() - 2 * rhs.left() + lhs.width() - rhs.width()) <
             lhs.width() + rhs.width() &&
         abs(2 * lhs.top() - 2 * rhs.top() + lhs.height() - rhs.height()) <
             lhs.height() + rhs.height();
}

Box Intersection(const Box& lhs, const Box& rhs) {
  Box box;
  box.set_left(std::max(lhs.left(), rhs.left()));
  box.set_top(std::max(lhs.top(), rhs.top()));

  const auto box_right =
      std::min(lhs.left() + lhs.width(), rhs.left() + rhs.width());
  const auto box_bottom =
      std::min(lhs.top() + lhs.height(), rhs.top() + rhs.height());
  box.set_width(box_right - box.left());
  box.set_height(box_bottom - box.top());

  return box;
}

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

}  // namespace geo

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
