#include <gtest/gtest.h>
#include <stan/math/fwd/scal.hpp>

TEST(MetaTraits, containsFvar) {
  using stan::contains_fvar;
  using stan::math::fvar;
  EXPECT_TRUE((contains_fvar<fvar<double>>::value));
  EXPECT_TRUE((contains_fvar<double, fvar<double>>::value));
  EXPECT_TRUE((contains_fvar<fvar<fvar<double>>>::value));
}
