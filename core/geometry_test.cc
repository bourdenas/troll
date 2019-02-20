#include "core/geometry.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "troll-test/test-util.h"

namespace troll {
namespace geo {

TEST_CASE("BoxContainsPoint", "[GeometryTest]") {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");
  const auto point = ParseProto<Vector>(R"(
    x: 15  y: 25)");

  REQUIRE(Contains(box, point));
}

TEST_CASE("BoxContainsPointIgnoresZAxis", "[GeometryTest]") {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");

  const auto front_point = ParseProto<Vector>(R"(
    x: 15  y: 25  z: 10)");
  REQUIRE(Contains(box, front_point));

  const auto back_point = ParseProto<Vector>(R"(
    x: 15  y: 25  z: -10)");
  REQUIRE(Contains(box, back_point));
}

TEST_CASE("BoxDoesNotContainPoint", "[GeometryTest]") {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");
  const auto point = ParseProto<Vector>(R"(
    x: 5  y: 5)");

  REQUIRE_FALSE(Contains(box, point));
}

TEST_CASE("BoxDoesNotContainBorderPoints", "[GeometryTest]") {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");

  const auto top_side_point = ParseProto<Vector>(R"(
    x: 15  y: 20)");
  REQUIRE_FALSE(Contains(box, top_side_point));

  const auto top_left_corner_point = ParseProto<Vector>(R"(
    x: 10  y: 20)");
  REQUIRE_FALSE(Contains(box, top_left_corner_point));

  const auto right_side_point = ParseProto<Vector>(R"(
    x: 40  y: 35)");
  REQUIRE_FALSE(Contains(box, top_side_point));

  const auto bottom_right_corner_point = ParseProto<Vector>(R"(
    x: 40  y: 60)");
  REQUIRE_FALSE(Contains(box, top_left_corner_point));
}

TEST_CASE("CollidesBoxesMatchExactly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");

  REQUIRE(Collide(lhs, lhs));
}

TEST_CASE("CollidesBoxesOverlapSignificantly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 15  top: 15  width: 10  height: 10)");

  REQUIRE(Collide(lhs, rhs));
  REQUIRE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesOverlapSlightly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 18  top: 28  width: 10  height: 10)");

  REQUIRE(Collide(lhs, rhs));
  REQUIRE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesDoNotOverlap", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 20  top: 5  width: 10  height: 10)");

  REQUIRE_FALSE(Collide(lhs, rhs));
  REQUIRE_FALSE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesTouchOnSides", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 0  width: 10  height: 10)");

  REQUIRE_FALSE(Collide(lhs, rhs));
  REQUIRE_FALSE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesTouchOnCorners", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 10)");

  REQUIRE_FALSE(Collide(lhs, rhs));
  REQUIRE_FALSE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesOneIncludesTheOther", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)");

  REQUIRE(Collide(lhs, rhs));
  REQUIRE(Collide(rhs, lhs));
}

TEST_CASE("CollidesBoxesOneIncludesTheOtherTouchingsides", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)");

  REQUIRE(Collide(lhs, rhs));
  REQUIRE(Collide(rhs, lhs));
}

TEST_CASE("IntersectionBoxesMatchExactly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");

  REQUIRE_THAT(Intersection(lhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)")));
}

TEST_CASE("IntersectionBoxesOverlapSignificantly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 15  top: 15  width: 10  height: 10)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 15  top: 15  width: 5  height: 10)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 15  top: 15  width: 5  height: 10)")));
}

TEST_CASE("IntersectionBoxesOverlapSlightly", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 18  top: 28  width: 10  height: 10)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 18  top: 28  width: 2  height: 2)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 18  top: 28  width: 2  height: 2)")));
}

TEST_CASE("IntersectionBoxesDoNotOverlap", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 20  top: 5  width: 10  height: 10)");

  // When input boxes don't overlap, the resulting box is invalid.
  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 20  top: 5  width: -10  height: 5)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 20  top: 5  width: -10  height: 5)")));
}

TEST_CASE("IntersectionBoxesTouchOnSides", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 0  width: 10  height: 10)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 0  width: 0  height: 10)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 0  width: 0  height: 10)")));
}

TEST_CASE("IntersectionBoxesTouchOnCorners", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 10)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 0  height: 0)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 0  height: 0)")));
}

TEST_CASE("IntersectionBoxesOneIncludesTheOther", "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)")));
}

TEST_CASE("IntersectionBoxesOneIncludesTheOtherTouchingsides",
          "[GeometryTest]") {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)");

  REQUIRE_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)")));
  REQUIRE_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)")));
}

}  // namespace geo
}  // namespace troll
