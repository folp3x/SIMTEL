#include "common/core/location/location.h"

#include <gtest/gtest.h>

class LocationTest : public ::testing::Test {
protected:
  common::Location<float> loc{{0, 0, 0}};
};

TEST_F(LocationTest, Move_VectorWithNoCoords) {
  std::vector<double> newLoc = {};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, Move_VectorWithOneCoord) {
  std::vector<double> newLoc = {7};
  loc.move(newLoc);

  auto expectedCoords = common::coords_t<float>{7, 0, 0};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithTwoCoords) {
  std::vector<double> newLoc = {7, 8};
  loc.move(newLoc);

  auto expectedCoords = common::coords_t<float>{7, 8, 0};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithThreeCoords) {
  std::vector<double> newLoc = {7, 8, 9};
  loc.move(newLoc);

  auto expectedCoords = common::coords_t<float>{7, 8, 9};

  EXPECT_TRUE(loc.getCoords() == expectedCoords);
}

TEST_F(LocationTest, Move_VectorWithFourCoords) {
  std::vector<double> newLoc = {1, 2, 3, 4};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, Move_Array) {
  auto newLoc = common::coords_t<float>{7, 8, 9};
  loc.move(newLoc);
  EXPECT_TRUE(loc.getCoords() == newLoc);
}

TEST_F(LocationTest, ToStr) {
  loc = common::Location<float>{{1, 2, 3}};
  EXPECT_TRUE(loc.toStr() == "(1, 2, 3)");
}

TEST_F(LocationTest, CoordsEqual_NoCoords) {
  std::vector<double> newLoc = {};
  EXPECT_THROW(loc.move(newLoc), std::invalid_argument);
}

TEST_F(LocationTest, CoordsEqual_OneCoordEqual) {
  loc = common::Location<float>{{7, 8, 9}};
  auto comparedCoords = std::vector<float>{7};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_TwoCoordsEqual) {
  loc = common::Location<float>{{7, 8, 9}};
  auto comparedCoords = std::vector<float>{7, 8};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_TwoCoordsNotEqual) {
  loc = common::Location<float>{{7, 0, 0}};
  auto comparedCoords = std::vector<float>{7, 8};
  EXPECT_FALSE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_ThreeCoordsEqual) {
  loc = common::Location<float>{{7, 8, 9}};
  auto comparedCoords = std::vector<float>{7, 8, 9};
  EXPECT_TRUE(loc.coordsEqual(comparedCoords));
}

TEST_F(LocationTest, CoordsEqual_FourCoordsEqual) {
  auto comparedCoords = std::vector<float>{1, 2, 3, 4};
  EXPECT_THROW(loc.coordsEqual(comparedCoords), std::invalid_argument);
}
