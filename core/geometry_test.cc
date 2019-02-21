#include "core/geometry.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "troll-test/test-util.h"

namespace troll {
namespace geo {

SCENARIO("Box contains a point", "[GeometryTest.Contains]") {
  GIVEN("a box") {
    const auto box =
        ParseProto<Box>(R"(left: 10  top: 20  width: 30  height: 40)");

    WHEN("a point is inside its bounds") {
      auto point = ParseProto<Vector>(R"(x: 15  y: 25)");
      THEN("Contains() succeeds") { REQUIRE(Contains(box, point)); }

      WHEN("the point is moved only in Z-axis") {
        point.set_z(10);
        THEN("Contains() still succeeds") { REQUIRE(Contains(box, point)); }
      }
    }

    WHEN("a point is outside its bounds") {
      const auto point = ParseProto<Vector>(R"(x: 5  y: 5)");
      THEN("Contains() fails") { REQUIRE_FALSE(Contains(box, point)); }
    }

    WHEN("a point is on its borders") {
      const auto top_side_point = ParseProto<Vector>(R"(x: 15  y: 20)");
      const auto top_left_corner_point = ParseProto<Vector>(R"(x: 10  y: 20)");
      const auto right_side_point = ParseProto<Vector>(R"(x: 40  y: 35)");
      const auto bottom_right_corner_point =
          ParseProto<Vector>(R"(x: 40  y: 60)");

      THEN("Contains() fails") {
        REQUIRE_FALSE(Contains(box, top_side_point));
        REQUIRE_FALSE(Contains(box, top_left_corner_point));
        REQUIRE_FALSE(Contains(box, top_side_point));
        REQUIRE_FALSE(Contains(box, top_left_corner_point));
      }
    }
  }
}

SCENARIO("Box collides with other boxes", "[GeometryTest.Collide]") {
  GIVEN("a box") {
    const auto lhs =
        ParseProto<Box>(R"(left: 10  top: 10  width: 10  height: 20)");

    WHEN("checked with itself") {
      const auto rhs = lhs;
      THEN("it collides with itself") { REQUIRE(Collide(lhs, rhs)); }
    }

    WHEN("overlaps with other boxes") {
      const auto rhs_large_overlap =
          ParseProto<Box>(R"(left: 15  top: 15  width: 10  height: 10)");
      const auto rhs_small_overlap =
          ParseProto<Box>(R"(left: 18  top: 28  width: 10  height: 10)");

      THEN("they collide") {
        REQUIRE(Collide(lhs, rhs_large_overlap));
        REQUIRE(Collide(lhs, rhs_small_overlap));
      }
    }

    WHEN("does not overlap with another") {
      const auto rhs =
          ParseProto<Box>(R"(left: 0  top: 0  width: 5  height: 5)");
      THEN("they do not collide") { REQUIRE_FALSE(Collide(lhs, rhs)); }
    }

    WHEN("touches sides with another box") {
      const auto rhs =
          ParseProto<Box>(R"(left: 20  top: 10  width: 5  height: 10)");

      THEN("they do not collide") {
        REQUIRE_FALSE(Collide(lhs, rhs));
        REQUIRE_FALSE(Collide(rhs, lhs));
      }
    }

    WHEN("touches corners with another box") {
      const auto rhs =
          ParseProto<Box>(R"(left: 0  top: 0  width: 10  height: 10)");

      THEN("they do not collide") {
        REQUIRE_FALSE(Collide(lhs, rhs));
        REQUIRE_FALSE(Collide(rhs, lhs));
      }
    }

    WHEN("fully contains another box") {
      const auto rhs =
          ParseProto<Box>(R"(left: 12  top: 12  width: 2  height: 2)");

      THEN("they collide") {
        REQUIRE(Collide(lhs, rhs));
        REQUIRE(Collide(rhs, lhs));
      }
    }

    WHEN("fully contains another box with which they also touch sides") {
      const auto rhs =
          ParseProto<Box>(R"(left: 10  top: 10  width: 2  height: 2)");

      THEN("they collide") {
        REQUIRE(Collide(lhs, rhs));
        REQUIRE(Collide(rhs, lhs));
      }
    }
  }
}

SCENARIO("Box intersects with other boxes", "[GeometryTest.Intersection") {
  GIVEN("a box") {
    const auto lhs =
        ParseProto<Box>(R"(left: 10  top: 10  width: 10  height: 20)");

    WHEN("checked with itself") {
      const auto rhs = lhs;
      THEN("intersects with identity") {
        REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(lhs));
      }
    }

    WHEN("overlaps with another") {
      const auto rhs_large_overlap =
          ParseProto<Box>(R"(left: 15  top: 15  width: 10  height: 10)");
      const auto rhs_small_overlap =
          ParseProto<Box>(R"(left: 18  top: 28  width: 10  height: 10)");
      CHECK(Collide(lhs, rhs_large_overlap));
      CHECK(Collide(lhs, rhs_small_overlap));

      THEN("they intersect") {
        REQUIRE_THAT(Intersection(lhs, rhs_large_overlap),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 15  top: 15  width: 5  height: 10)")));
        REQUIRE_THAT(Intersection(lhs, rhs_small_overlap),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 18  top: 28  width: 2  height: 2)")));
      }
    }

    WHEN("does not overlap with another") {
      const auto rhs =
          ParseProto<Box>(R"(left: 0  top: 0  width: 5  height: 5)");
      CHECK_FALSE(Collide(lhs, rhs));

      THEN("intersection box is invalid") {
        REQUIRE_THAT(Intersection(lhs, rhs),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 10  top: 10  width: -5  height: -5)")));
      }
    }

    WHEN("touches sides with another box") {
      const auto rhs =
          ParseProto<Box>(R"(left: 20  top: 10  width: 5  height: 10)");

      THEN("touching dimension of intersection is 0") {
        REQUIRE_THAT(Intersection(lhs, rhs),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 20  top: 10  width: 0  height: 10)")));
        REQUIRE_THAT(Intersection(rhs, lhs),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 20  top: 10  width: 0  height: 10)")));
      }
    }

    WHEN("touches corners with another box") {
      const auto rhs = ParseProto<Box>(R"(
          left: 0  top: 0  width: 10  height: 10)");

      THEN("all intersection dimensions are 0") {
        REQUIRE_THAT(Intersection(lhs, rhs),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 10  top: 10  width: 0  height: 0)")));
        REQUIRE_THAT(Intersection(rhs, lhs),
                     EqualsProto(ParseProto<Box>(
                         R"(left: 10  top: 10  width: 0  height: 0)")));
      }
    }

    WHEN("fully contains another box") {
      const auto rhs = ParseProto<Box>(R"(
          left: 12  top: 12  width: 2  height: 2)");

      THEN("intersection is the contained box") {
        REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(rhs));
        REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(rhs));
      }
    }

    WHEN("fully contains another box with which they also touch sides") {
      const auto rhs = ParseProto<Box>(R"(
          left: 10  top: 10  width: 2  height: 2)");

      THEN("intersection is the contained box") {
        REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(rhs));
        REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(rhs));
      }
    }
  }
}

}  // namespace geo
}  // namespace troll
