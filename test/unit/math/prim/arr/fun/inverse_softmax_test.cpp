#include <stan/math/prim/arr.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <vector>

TEST(MathFunctions, inverse_softmax_exception) {
  std::vector<double> simplex(2);
  std::vector<double> y(3);
  EXPECT_THROW(stan::math::inverse_softmax<std::vector<double> >(simplex, y),
               std::invalid_argument);
}

TEST(MathFunctions, inverse_softmax) {
  std::vector<double> simplex(2);
  std::vector<double> y(2);

  simplex[0] = 0.2;
  simplex[1] = 0.8;

  stan::math::inverse_softmax(simplex, y);
  EXPECT_FLOAT_EQ(log(0.2), y[0]);
  EXPECT_FLOAT_EQ(log(0.8), y[1]);
}

TEST(MathFunctions, inverse_softmax_nan) {
  double nan = std::numeric_limits<double>::quiet_NaN();

  std::vector<double> simplex(2);
  std::vector<double> y(2);

  simplex[0] = nan;
  simplex[1] = nan;

  stan::math::inverse_softmax(simplex, y);
  EXPECT_TRUE(std::isnan(y[0]));
  EXPECT_TRUE(std::isnan(y[1]));
}