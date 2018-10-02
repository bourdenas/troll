#include "core/geometry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "troll-test/test-util.h"

namespace troll {
namespace geo {

class GeometryTest : public testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(GeometryTest, BoxContainsPoint) {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");
  const auto point = ParseProto<Vector>(R"(
    x: 15  y: 25)");

  EXPECT_TRUE(Contains(box, point));
}

TEST_F(GeometryTest, BoxContainsPointIgnoresZAxis) {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");

  const auto front_point = ParseProto<Vector>(R"(
    x: 15  y: 25  z: 10)");
  EXPECT_TRUE(Contains(box, front_point));

  const auto back_point = ParseProto<Vector>(R"(
    x: 15  y: 25  z: -10)");
  EXPECT_TRUE(Contains(box, back_point));
}

TEST_F(GeometryTest, BoxDoesNotContainPoint) {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");
  const auto point = ParseProto<Vector>(R"(
    x: 5  y: 5)");

  EXPECT_FALSE(Contains(box, point));
}

TEST_F(GeometryTest, BoxDoesNotContainBorderPoints) {
  const auto box = ParseProto<Box>(R"(
    left: 10  top: 20  width: 30  height: 40)");

  const auto top_side_point = ParseProto<Vector>(R"(
    x: 15  y: 20)");
  EXPECT_FALSE(Contains(box, top_side_point));

  const auto top_left_corner_point = ParseProto<Vector>(R"(
    x: 10  y: 20)");
  EXPECT_FALSE(Contains(box, top_left_corner_point));

  const auto right_side_point = ParseProto<Vector>(R"(
    x: 40  y: 35)");
  EXPECT_FALSE(Contains(box, top_side_point));

  const auto bottom_right_corner_point = ParseProto<Vector>(R"(
    x: 40  y: 60)");
  EXPECT_FALSE(Contains(box, top_left_corner_point));
}

TEST_F(GeometryTest, CollidesBoxesMatchExactly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");

  EXPECT_TRUE(Collide(lhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesOverlapSignificantly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 15  top: 15  width: 10  height: 10)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesOverlapSlightly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 18  top: 28  width: 10  height: 10)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesDoNotOverlap) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 20  top: 5  width: 10  height: 10)");

  EXPECT_FALSE(Collide(lhs, rhs));
  EXPECT_FALSE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesTouchOnSides) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 0  width: 10  height: 10)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesTouchOnCorners) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 10)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesOneIncludesTheOther) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, CollidesBoxesOneIncludesTheOtherTouchingsides) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)");

  EXPECT_TRUE(Collide(lhs, rhs));
  EXPECT_TRUE(Collide(rhs, lhs));
}

TEST_F(GeometryTest, IntersectionBoxesMatchExactly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");

  EXPECT_THAT(Intersection(lhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)")));
}

TEST_F(GeometryTest, IntersectionBoxesOverlapSignificantly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 15  top: 15  width: 10  height: 10)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 15  top: 15  width: 5  height: 10)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 15  top: 15  width: 5  height: 10)")));
}

TEST_F(GeometryTest, IntersectionBoxesOverlapSlightly) {
  const auto lhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 18  top: 28  width: 10  height: 10)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 18  top: 28  width: 2  height: 2)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 18  top: 28  width: 2  height: 2)")));
}

TEST_F(GeometryTest, IntersectionBoxesDoNotOverlap) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 20  top: 5  width: 10  height: 10)");

  // When input boxes don't overlap, the resulting box is invalid.
  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 20  top: 5  width: -10  height: 5)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 20  top: 5  width: -10  height: 5)")));
}

TEST_F(GeometryTest, IntersectionBoxesTouchOnSides) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 0  width: 10  height: 10)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 0  width: 0  height: 10)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 0  width: 0  height: 10)")));
}

TEST_F(GeometryTest, IntersectionBoxesTouchOnCorners) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 10  height: 10)");
  const auto rhs = ParseProto<Box>(R"(
    left: 10  top: 10  width: 10  height: 10)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 0  height: 0)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 10  top: 10  width: 0  height: 0)")));
}

TEST_F(GeometryTest, IntersectionBoxesOneIncludesTheOther) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 10  height: 10)")));
}

TEST_F(GeometryTest, IntersectionBoxesOneIncludesTheOtherTouchingsides) {
  const auto lhs = ParseProto<Box>(R"(
    left: 0  top: 0  width: 20  height: 20)");
  const auto rhs = ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)");

  EXPECT_THAT(Intersection(lhs, rhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)")));
  EXPECT_THAT(Intersection(rhs, lhs), EqualsProto(ParseProto<Box>(R"(
    left: 5  top: 5  width: 15  height: 15)")));
}

}  // namespace geo
}  // namespace troll
