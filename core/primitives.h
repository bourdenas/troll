#ifndef TROLL_CORE_PRIMITIVES_H_
#define TROLL_CORE_PRIMITIVES_H_

#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>

namespace troll {

using Point =
    boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian>;
using Box = boost::geometry::model::box<Point>;

using RGBa = boost::geometry::model::point<unsigned char, 4,
                                           boost::geometry::cs::cartesian>;

}  // namespace troll

#endif  // TROLL_CORE_PRIMITIVES_H_
