#include <gtest/gtest.h>
#include <sstream>
#include <stan/math/rev/core.hpp>

TEST(AgradRev, insertion_operator) {
  stan::math::vari v(5);
  std::stringstream ss;
  ss << &v;
  EXPECT_EQ("5:0", ss.str());
}
