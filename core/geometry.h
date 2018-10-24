#ifndef TROLL_CORE_GEOMETRY_H_
#define TROLL_CORE_GEOMETRY_H_

#include "proto/primitives.pb.h"
#include "proto/scene-node.pb.h"

namespace troll {
namespace geo {

// Returns true if |box| contains point |v|. Border points are not contained.
bool Contains(const Box& box, const Vector& v);

// Returns true if the two input boxes overlap. Touching sides is no collision.
bool Collide(const Box& lhs, const Box& rhs);

// Returns the box defined by the intersection of input boxes. If the boxes do
// not collide, the returned box is invalid.
Box Intersection(const Box& lhs, const Box& rhs);

double VectorLength(const Vector& v);
double VectorSquaredLength(const Vector& v);

// Normalise vector in place.
void VectorNormalise(Vector* v);

}  // namespace geo

Vector operator+(const Vector& left, const Vector& right);
Vector operator-(const Vector& left, const Vector& right);
Vector operator*(const Vector& v, double a);

}  // namespace troll

#endif  // TROLL_CORE_GEOMETRY_H_
