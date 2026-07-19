#include "core/location/location/location.h"

#include <gtest/gtest.h>

class LocationTest : public testing::Test {
protected:
  static const size_t TESTED_COORDS_COUNT = 3;
  common::Location<float, TESTED_COORDS_COUNT> loc{{0, 0, 0}};
};

TEST_F(LocationTest, Move_VectorWithNoCoords) {
  std::vector<float> newLoc{};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, Move_VectorWithOneCoord) {
  std::vector<float> newLoc = {7};
  loc.move(newLoc);

  common::coords_t<float, TESTED_COORDS_COUNT> expectedCoords{7, 0, 0};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithTwoCoords) {
  std::vector<float> newLoc = {7, 8};
  loc.move(newLoc);

  common::coords_t<float, TESTED_COORDS_COUNT> expectedCoords{7, 8, 0};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithThreeCoords) {
  std::vector<float> newLoc = {7, 8, 9};
  loc.move(newLoc);

  common::coords_t<float, TESTED_COORDS_COUNT> expectedCoords{7, 8, 9};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithFourCoords) {
  std::vector<float> newLoc = {1, 2, 3, 4};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, Move_Array) {
  common::coords_t<float, TESTED_COORDS_COUNT> newLoc{7, 8, 9};
  loc.move(newLoc);
  EXPECT_TRUE(loc.getCoords() == newLoc);
}

TEST_F(LocationTest, ToStr) {
  loc = common::Location<float, TESTED_COORDS_COUNT>{{1, 2, 3}};
  EXPECT_TRUE(loc.toStr() == "(1, 2, 3)");
}

TEST_F(LocationTest, CoordsEqual_NoCoords) {
  std::vector<float> newLoc{};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, CoordsEqual_OneCoordEqual) {
  loc = common::Location<float, TESTED_COORDS_COUNT>{{7, 8, 9}};
  std::vector<float> comparedCoords{7};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_TwoCoordsEqual) {
  loc = common::Location<float, TESTED_COORDS_COUNT>{{7, 8, 9}};
  std::vector<float> comparedCoords{7, 8};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_TwoCoordsNotEqual) {
  loc = common::Location<float, TESTED_COORDS_COUNT>{{7, 0, 0}};
  std::vector<float> comparedCoords{7, 8};
  EXPECT_FALSE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_ThreeCoordsEqual) {
  loc = common::Location<float, TESTED_COORDS_COUNT>{{7, 8, 9}};
  std::vector<float> comparedCoords{7, 8, 9};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_FourCoordsEqual) {
  std::vector<float> comparedCoords{1, 2, 3, 4};
  EXPECT_THROW(loc.coordsEqual(comparedCoords), std::invalid_argument);
}
