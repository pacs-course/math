#include <test/unit/math/test_ad.hpp>
#include <stan/math/rev/core.hpp>
#include <test/unit/math/rev/mat/fun/util.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

typedef stan::math::var var_t;
typedef std::complex<stan::math::var> cvar_t;
typedef std::complex<double> cdouble_t;
typedef Eigen::Matrix<stan::math::var, -1, -1> mvar_t;

template <typename T>
void expect_identity_matrix(const T& I) {
  for (int i = 0; i < I.rows(); ++i) {
    EXPECT_NEAR(1, I(i, i).val(), 1e-8);
    for (int j = 0; j < i; ++j) {
      EXPECT_NEAR(0, I(i, j).val(), 1e-8);
      EXPECT_NEAR(0, I(j, i).val(), 1e-8);
    }
  }
}

template <typename F>
void expect_reduction(const F& f) {
  cvar_t x(1, 2);
  var_t y = f(x);

  cdouble_t xd(1, 2);
  double yd = f(xd);
  EXPECT_FLOAT_EQ(yd, y.val());
}

void expect_complex(double re, double im, const cvar_t& y) {
  using stan::math::is_nan;
  if (is_nan(re)) {
    EXPECT_TRUE(is_nan(y.real()))
        << "expected = " << std::complex<double>{re, im} << "; found = " << y
        << std::endl;
  } else {
    if (std::fabs(re) < 1e100 && std::fabs(y.real().val()) < 1e100) {
      EXPECT_NEAR(re, y.real().val(), 1e-7)
          << "expected = (" << re << ", " << im << ")"
          << "; found = " << y << std::endl;
    } else {
      EXPECT_FLOAT_EQ(re, y.real().val())
          << "expected = (" << re << ", " << im << ")"
          << "; found = " << y << std::endl;
    }
  }
  if (is_nan(im)) {
    EXPECT_TRUE(is_nan(y.imag().val()))
        << "expected = (" << re << ", " << im << ")"
        << "; found = " << y << std::endl;
  } else {
    if (std::fabs(im) < 1e100 && std::fabs(y.imag().val()) < 1e100) {
      EXPECT_NEAR(im, y.imag().val(), 1e-7)
          << "expected = (" << re << ", " << im << ")"
          << "; found = " << y << std::endl;
    } else {
      EXPECT_FLOAT_EQ(im, y.imag().val())
          << "expected = (" << re << ", " << im << ")"
          << "; found = " << y << std::endl;
    }
  }
}
void expect_complex(const cdouble_t& x, const cvar_t& y) {
  expect_complex(x.real(), x.imag(), y);
}

std::vector<double> common_non_neg_vals() {
  double inf = std::numeric_limits<double>::infinity();
  double nan = std::numeric_limits<double>::quiet_NaN();
  double pos_zero = 0.0;
  double neg_zero = -0.0;
  return {0.0, 1.3, 2.1};
}

std::vector<double> common_vals() {
  double inf = std::numeric_limits<double>::infinity();
  double nan = std::numeric_limits<double>::quiet_NaN();
  double pos_zero = 0.0;
  double neg_zero = -0.0;
  return {-4, -2.5, -1.5, -0.3, -0.0, 0.0, 1.3, 2.1, 3.9};
}

template <typename F>
void expect_complex_common(const F& f) {
  for (double a : common_vals()) {
    for (double b : common_vals()) {
      // std:: cout << "z = " << cdouble_t{a , b} << std::endl;
      expect_complex(f(cdouble_t{a, b}), f(cvar_t{a, b}));
    }
  }
}

template <typename F>
void expect_complex_common_binary(const F& f) {
  using stan::math::pow;
  for (double x1 : common_non_neg_vals()) {
    for (double y1 : common_non_neg_vals()) {
      for (double x2 : common_non_neg_vals()) {
        // std::cout << "x1 = " << x1 << "; y1 = "
        // << y1 << "; x2 = " << x2 << std::endl;
        expect_complex(f(cdouble_t{x1, y1}, x2), f(cvar_t{x1, y1}, var_t{x2}));
        expect_complex(f(cdouble_t{x1, y1}, x2), f(cvar_t{x1, y1}, x2));
        expect_complex(f(x2, cdouble_t{x1, y1}), f(var_t{x2}, cvar_t{x1, y1}));
        expect_complex(f(x2, cdouble_t{x1, y1}), f(x2, cvar_t{x1, y1}));
        for (double y2 : common_non_neg_vals()) {
          // std::cout << "    y2 = " << y2 << std::endl;
          expect_complex(f(cdouble_t{x1, y1}, cdouble_t{x2, y2}),
                         f(cvar_t{x1, y1}, cvar_t{x2, y2}));
        }
      }
    }
  }
}

template <typename F>
void expect_compound_assign_operator(const F& f) {
  // cvar += var
  cdouble_t ad(1, 2);
  f(ad, 3);
  cvar_t a(1, 2);
  var_t b = 3;
  f(a, b);
  expect_complex(ad, a);
  auto ptr1 = &a;
  auto ptr2 = &(a += b);
  EXPECT_EQ(ptr1, ptr2);

  // cvar += cdouble
  cvar_t e(1, 2);
  cdouble_t ed(1, 2);
  cdouble_t bd(3, 4);
  f(ed, bd);
  f(e, bd);
  expect_complex(ed, e);
  ptr1 = &e;
  ptr2 = &(e += bd);
  EXPECT_EQ(ptr1, ptr2);

  // cvar += cvar
  cdouble_t gd(1, 2);
  cdouble_t hd(3, 4);
  f(gd, hd);
  cvar_t g(1, 2);
  cvar_t h(3, 4);
  f(g, h);
  expect_complex(gd, g);
  ptr1 = &g;
  ptr2 = &(g += h);
  EXPECT_EQ(ptr1, ptr2);

  // cvar += double
  cdouble_t kd(1, 2);
  f(kd, 3.0);
  cvar_t k(1, 2);
  f(k, 3.0);
  expect_complex(kd, k);
  ptr1 = &k;
  ptr2 = &(k += 3.0);
  EXPECT_EQ(ptr1, ptr2);

  // cvar += int
  cdouble_t jd(1, 2);
  f(jd, 3);
  cvar_t j(1, 2);
  f(j, 3);
  expect_complex(jd, j);
  ptr1 = &j;
  ptr2 = &(j += 3);
  EXPECT_EQ(ptr1, ptr2);
}

TEST(mathRevCore, stdIteratorTraits) {
  using itv_t = std::iterator_traits<var_t>;
  var_t v = 1.3;

  itv_t::value_type v2 = v;
  EXPECT_FLOAT_EQ(1.3, v2.val());

  itv_t::pointer v_ptr = &v;
  EXPECT_FLOAT_EQ(1.3, v_ptr->val());
  *v_ptr += 2.1;
  EXPECT_FLOAT_EQ(3.4, v.val());

  itv_t::reference v_ref = v;
  v_ref = 9.7;
  EXPECT_FLOAT_EQ(9.7, v.val());

  itv_t::difference_type a = v_ptr - v_ptr;
}
TEST(mathRevCore, signbit) {
  EXPECT_TRUE(signbit(-std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(signbit(std::numeric_limits<var_t>::infinity()));
  EXPECT_TRUE(signbit(var_t{-2}));
  EXPECT_FALSE(signbit(var_t{0}));
  EXPECT_FALSE(signbit(var_t{3.9}));
}
TEST(mathRevCore, isinf) {
  EXPECT_FALSE(isinf(std::numeric_limits<var_t>::quiet_NaN()));
  EXPECT_TRUE(isinf(std::numeric_limits<var_t>::infinity()));
  EXPECT_TRUE(isinf(-std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isinf(var_t{-1}));
  EXPECT_FALSE(isinf(var_t{0}));
  EXPECT_FALSE(isinf(var_t{3e27}));
}
TEST(mathRevCore, isfinite) {
  EXPECT_FALSE(isfinite(std::numeric_limits<var_t>::quiet_NaN()));
  EXPECT_FALSE(isfinite(std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isfinite(-std::numeric_limits<var_t>::infinity()));
  EXPECT_TRUE(isfinite(var_t{-1}));
  EXPECT_TRUE(isfinite(var_t{0}));
  EXPECT_TRUE(isfinite(var_t{3e27}));
}
TEST(mathRevCore, isnan) {
  EXPECT_TRUE(isnan(std::numeric_limits<var_t>::quiet_NaN()));
  EXPECT_FALSE(isnan(std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isnan(-std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isnan(var_t{-1}));
  EXPECT_FALSE(isnan(var_t{0}));
  EXPECT_FALSE(isnan(var_t{3e27}));
}
TEST(mathRevCore, isnormal) {
  EXPECT_FALSE(isnormal(std::numeric_limits<var_t>::quiet_NaN()));
  EXPECT_FALSE(isnormal(std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isnormal(-std::numeric_limits<var_t>::infinity()));
  EXPECT_FALSE(isnormal(var_t{0}));
  EXPECT_TRUE(isnormal(var_t{-1}));
  EXPECT_TRUE(isnormal(var_t{3e27}));
}
TEST(mathRevCore, copysignScalar) {
  for (double i = -3; i < 3; ++i) {
    for (double j = -3; j < 3; ++j) {
      std::cout << "i = " << i << "; j " << j << std::endl;
      EXPECT_FLOAT_EQ(copysign(i, j), copysign(var_t{i}, var_t{j}).val());
    }
  }
}
TEST(mathRevCore, copysignComplex) {
  // this one's not in std:
  for (double i = -1; i < 2; ++i) {
    for (double j = -1; j < 2; ++j) {
      cdouble_t a{i, j};
      for (double k = -1; k < 2; ++k) {
        for (double l = -1; l < 2; ++l) {
          using stan::math::copysign;
          cdouble_t b{k, l};
          cvar_t av{a};
          cvar_t bv{b};
          cdouble_t c = copysign(a, b);
          cvar_t cv = copysign(av, bv);
          EXPECT_FLOAT_EQ(std::copysign(i, k), c.real());
          EXPECT_FLOAT_EQ(std::copysign(j, l), c.imag());
          EXPECT_FLOAT_EQ(std::copysign(i, k), cv.real().val());
          EXPECT_FLOAT_EQ(std::copysign(j, l), cv.imag().val());
        }
      }
    }
  }
}

TEST(mathRevCore, iTimes) {
  for (double i = -1; i < 2; ++i) {
    for (double j = -1; j < 2; ++j) {
      cdouble_t a(i, j);
      auto i_complex = cdouble_t{0, 1};
      auto expected = i_complex * a;
      auto found = stan::math::i_times(a);  // qualify for non-stan args
      EXPECT_FLOAT_EQ(expected.real(), found.real());
      EXPECT_FLOAT_EQ(expected.imag(), found.imag());

      cvar_t av(i, j);
      auto foundv = i_times(av);  // ADL for stan args
      EXPECT_FLOAT_EQ(expected.real(), foundv.real().val());
      EXPECT_FLOAT_EQ(expected.imag(), foundv.imag().val());
    }
  }
}

TEST(mathRevCore, negITimes) {
  for (double i = -1; i < 2; ++i) {
    for (double j = -1; j < 2; ++j) {
      cdouble_t a(i, j);
      auto neg_i_complex = cdouble_t{0, -1};
      auto expected = neg_i_complex * a;
      auto found = stan::math::neg_i_times(a);
      EXPECT_FLOAT_EQ(expected.real(), found.real());
      EXPECT_FLOAT_EQ(expected.imag(), found.imag());

      cvar_t av(i, j);
      auto foundv = neg_i_times(av);
      EXPECT_FLOAT_EQ(expected.real(), foundv.real().val());
      EXPECT_FLOAT_EQ(expected.imag(), foundv.imag().val());
    }
  }
}

TEST(mathRevCore, stdComplexConstructor1) {
  // constructor (1), no defaults
  var_t x = 1;
  var_t y = 2;
  cvar_t z{x, y};
  expect_complex(1, 2, z);

  // constructor (1), default imaginary
  cvar_t a{x};
  expect_complex(1, 0, a);

  // verifies std::complex<var>() produces (0, 0)
  // constructor (1), default real and imaginary
  cvar_t b{};
  expect_complex(0, 0, b);

  // direct double and mixed
  cvar_t c{x, 2.0};
  expect_complex(1, 2, c);

  cvar_t d{1.0, y};
  expect_complex(1, 2, d);

  cvar_t e{1.0, 2.0};
  expect_complex(1, 2, e);

  // int and mixed
  cvar_t f{x, 2};
  expect_complex(1, 2, f);

  cvar_t g{1, y};
  expect_complex(1, 2, g);

  cvar_t h{1, 2};
  expect_complex(1, 2, h);

  cvar_t j{1.0, 2};
  expect_complex(1, 2, j);

  cvar_t k{1, 2.0};
  expect_complex(1, 2, k);

  cvar_t l{1.0, 2.0};
  expect_complex(1, 2, l);
}
TEST(mathRevCore, stdComplexConstructor2) {
  var_t x = 1;
  var_t y = 2;
  cvar_t a{x, y};
  cvar_t b{a};
  // require pimpl equality
  EXPECT_EQ(a.real().vi_, b.real().vi_);
  EXPECT_EQ(a.imag().vi_, b.imag().vi_);
}
TEST(mathRevCore, stdComplexConstructor3) {
  cdouble_t a(1, 2);
  cvar_t b(a);
  expect_complex(1, 2, b);
}
TEST(mathRevCore, stdComplexReal1) {
  cvar_t a(3, -1);
  EXPECT_FLOAT_EQ(3, a.real().val());
}
TEST(mathRevCore, stdComplexReal2) {
  // test double, int, var args
  cvar_t a(3, -1);
  a.real(2.7);
  EXPECT_FLOAT_EQ(2.7, a.real().val());
  EXPECT_FLOAT_EQ(-1, a.imag().val());

  a.real(2);
  EXPECT_FLOAT_EQ(2, a.real().val());
  EXPECT_FLOAT_EQ(-1, a.imag().val());

  var_t c(5);
  a.real(c);
  EXPECT_EQ(c.vi_, a.real().vi_);
  EXPECT_FLOAT_EQ(5, a.real().val());
  EXPECT_FLOAT_EQ(-1, a.imag().val());
}
TEST(mathRevCore, stdComplexImag1) {
  cvar_t a(3, -1);
  EXPECT_FLOAT_EQ(-1, a.imag().val());
}
TEST(mathRevCore, stdComplexImag2) {
  // test double, int, var args
  cvar_t a(3, -1);
  a.imag(2.7);
  EXPECT_FLOAT_EQ(2.7, a.imag().val());
  EXPECT_FLOAT_EQ(3, a.real().val());

  a.imag(152);
  EXPECT_FLOAT_EQ(152, a.imag().val());
  EXPECT_FLOAT_EQ(3, a.real().val());

  var_t c(42);
  a.imag(c);
  EXPECT_FLOAT_EQ(42, a.imag().val());
  EXPECT_FLOAT_EQ(3, a.real().val());
}
TEST(mathRevCore, stdComplexOperatorEquals1and2and3) {
  auto f = [](auto& x, const auto& y) { x = y; };
  expect_compound_assign_operator(f);
}

TEST(mathRevCore, stdComplexOperatorAddEquals1and5) {
  auto f = [](auto& x1, const auto& x2) { return x1 += x2; };
  expect_compound_assign_operator(f);
}
TEST(mathRevCore, stdComplexOperatorSubtractEquals2and6) {
  auto f = [](auto& x1, const auto& x2) { return x1 -= x2; };
  expect_compound_assign_operator(f);
}
TEST(mathRevCore, stdComplexOperatorMultiplyEquals3and7) {
  auto f = [](auto& x1, const auto& x2) { return x1 *= x2; };
  expect_compound_assign_operator(f);
}
TEST(mathRevCore, stdComplexOperatorDivideEquals4and8) {
  auto f = [](auto& x1, const auto& x2) { return x1 /= x2; };
}
TEST(mathRevCore, stdComplexOperatorUnaryPlus1) {
  cdouble_t ad(1, 2);
  cdouble_t bd = +ad;
  cvar_t a(1, 2);
  cvar_t b = +a;
  expect_complex(bd, b);
  // expect pointer equality before and after
  EXPECT_EQ(a.real().vi_, b.real().vi_);
}
TEST(mathRevCore, stdComplexOperatorUnaryNegative2) {
  cdouble_t ad(1, 2);
  cdouble_t bd = -ad;
  cvar_t a(1, 2);
  cvar_t b = -a;
  expect_complex(bd, b);
}
TEST(mathRevCore, stdComplexOperatorAdd1) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  cdouble_t cd = ad + bd;
  cvar_t a(1, 2);
  cvar_t b(3, 7);
  cvar_t c = a + b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorAdd2) {
  cdouble_t ad(1, 2);
  double bd = 3;
  cdouble_t cd = ad + bd;
  cvar_t a(1, 2);
  var_t b = 3;
  cvar_t c = a + b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorAdd3) {
  double ad = 1;
  cdouble_t bd(3, 7);
  cdouble_t cd = ad + bd;
  var_t a = 1;
  cvar_t b(3, 7);
  cvar_t c = a + b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorSubtract4) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  cdouble_t cd = ad - bd;
  cvar_t a(1, 2);
  cvar_t b(3, 7);
  cvar_t c = a - b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorSubtract5) {
  cdouble_t ad(1, 2);
  double bd = 3;
  cdouble_t cd = ad - bd;
  cvar_t a(1, 2);
  var_t b = 3;
  cvar_t c = a - b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorSubtract6) {
  double ad = 1;
  cdouble_t bd(3, 7);
  cdouble_t cd = ad - bd;
  var_t a = 1;
  cvar_t b(3, 7);
  cvar_t c = a - b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorMultiply7) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  cdouble_t cd = ad * bd;
  cvar_t a(1, 2);
  cvar_t b(3, 7);
  cvar_t c = a * b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorMultiply8) {
  cdouble_t ad(1, 2);
  double bd = 3;
  cdouble_t cd = ad * bd;
  cvar_t a(1, 2);
  var_t b = 3;
  cvar_t c = a * b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorMultiply9) {
  double ad = 1;
  cdouble_t bd(3, 7);
  cdouble_t cd = ad * bd;
  var_t a = 1;
  cvar_t b(3, 7);
  cvar_t c = a * b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorDivide10) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  cdouble_t cd = ad / bd;
  cvar_t a(1, 2);
  cvar_t b(3, 7);
  cvar_t c = a / b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorDivide11) {
  cdouble_t ad(1, 2);
  double bd = 3;
  cdouble_t cd = ad / bd;
  cvar_t a(1, 2);
  var_t b = 3;
  cvar_t c = a / b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorDivide12) {
  double ad = 1;
  cdouble_t bd(3, 7);
  cdouble_t cd = ad / bd;
  var_t a = 1;
  cvar_t b(3, 7);
  cvar_t c = a / b;
  expect_complex(cd, c);
}
TEST(mathRevCore, stdComplexOperatorCompare1) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  bool cd = ad == bd;
  bool dd = ad == ad;

  cvar_t a(1, 2);
  cvar_t b(3, 7);
  bool c = a == b;
  bool d = a == a;
  EXPECT_EQ(cd, c);
  EXPECT_EQ(dd, d);
}
TEST(mathRevCore, stdComplexOperatorCompare2) {
  cdouble_t ad(1, 0);
  cdouble_t bd(2, 3);
  double cd = 1;
  double dd = 3.2;

  bool ed = ad == cd;
  bool fd = ad == dd;
  bool gd = bd == cd;
  bool hd = bd == dd;

  cvar_t a(1, 0);
  cvar_t b(2, 3);
  var_t c = 1;
  var_t d = 3.2;

  bool e = a == c;
  bool f = a == d;
  bool g = b == c;
  bool h = b == d;
  EXPECT_EQ(ed, e);
  EXPECT_EQ(fd, f);
  EXPECT_EQ(gd, g);
  EXPECT_EQ(hd, h);
}
TEST(mathRevCore, stdComplexOperatorCompare3) {
  cdouble_t ad(1, 0);
  cdouble_t bd(2, 3);
  double cd = 1;
  double dd = 3.2;

  bool ed = cd == ad;
  bool fd = cd == bd;
  bool gd = dd == ad;
  bool hd = dd == bd;

  cvar_t a(1, 0);
  cvar_t b(2, 3);
  var_t c = 1;
  var_t d = 3.2;

  bool e = c == a;
  bool f = c == b;
  bool g = d == a;
  bool h = d == b;
  EXPECT_EQ(ed, e);
  EXPECT_EQ(fd, f);
  EXPECT_EQ(gd, g);
  EXPECT_EQ(hd, h);
}
TEST(mathRevCore, stdComplexOperatorCompareUneq4) {
  cdouble_t ad(1, 2);
  cdouble_t bd(3, 7);
  bool cd = ad != bd;
  bool dd = ad != ad;

  cvar_t a(1, 2);
  cvar_t b(3, 7);
  bool c = a != b;
  bool d = a != a;
  EXPECT_EQ(cd, c);
  EXPECT_EQ(dd, d);
}
TEST(mathRevCore, stdComplexOperatorCompareUneq5) {
  cdouble_t ad(1, 0);
  cdouble_t bd(2, 3);
  double cd = 1;
  double dd = 3.2;

  bool ed = ad != cd;
  bool fd = ad != dd;
  bool gd = bd != cd;
  bool hd = bd != dd;

  cvar_t a(1, 0);
  cvar_t b(2, 3);
  var_t c = 1;
  var_t d = 3.2;

  bool e = a != c;
  bool f = a != d;
  bool g = b != c;
  bool h = b != d;
  EXPECT_EQ(ed, e);
  EXPECT_EQ(fd, f);
  EXPECT_EQ(gd, g);
  EXPECT_EQ(hd, h);
}
TEST(mathRevCore, stdComplexOperatorCompareUneq6) {
  cdouble_t ad(1, 0);
  cdouble_t bd(2, 3);
  double cd = 1;
  double dd = 3.2;

  bool ed = cd != ad;
  bool fd = cd != bd;
  bool gd = dd != ad;
  bool hd = dd != bd;

  cvar_t a(1, 0);
  cvar_t b(2, 3);
  var_t c = 1;
  var_t d = 3.2;

  bool e = c != a;
  bool f = c != b;
  bool g = d != a;
  bool h = d != b;
  EXPECT_EQ(ed, e);
  EXPECT_EQ(fd, f);
  EXPECT_EQ(gd, g);
  EXPECT_EQ(hd, h);
}
// the operator streams use the compiler-supplied implementations
// this is generally unspecified behavior, but seems to work
TEST(mathRevCore, stanMathOperatorStreamOut1) {
  cdouble_t ad(1, 2);
  cvar_t a(1, 2);

  std::stringstream ssd;
  ssd << ad;
  std::string sd = ssd.str();
  std::stringstream ss;
  ss << a;
  std::string s = ss.str();
  EXPECT_EQ(sd, s);
}
TEST(mathRevCore, stanMathOperatorStreamIn2) {
  std::stringstream s1;
  s1 << "(1, 2)";
  cvar_t a;
  s1 >> a;
  expect_complex(1, 2, a);

  std::stringstream s2;
  s2 << "(1)";
  s2 >> a;
  expect_complex(1, 0, a);

  std::stringstream s3;
  s3 << "1";
  s3 >> a;
  expect_complex(1, 0, a);
}
TEST(mathRevCore, stdRealExternal1) {
  var_t a = 1;
  var_t b = 2;
  cvar_t c(a, b);
  var_t ca = std::real(c);
  EXPECT_EQ(a.vi_, ca.vi_);
}
TEST(mathRevCore, stdImagExternal1) {
  var_t a = 1;
  var_t b = 2;
  cvar_t c(a, b);
  var_t cb = std::imag(c);
  EXPECT_EQ(b.vi_, cb.vi_);
}
TEST(mathRevCore, stdAbsExternal1) {
  cdouble_t ad(1, 2);
  double bd = std::abs(ad);
  cvar_t a(1, 2);
  var_t b = std::abs(a);
  EXPECT_FLOAT_EQ(bd, b.val());
}
TEST(mathRevCore, stdArgExternal1) {
  cdouble_t ad(1, 2);
  double bd = std::arg(ad);
  cvar_t a(1, 2);
  var_t b = std::arg(a);
  EXPECT_FLOAT_EQ(bd, b.val());
}
TEST(mathRevCore, stdNormExternal1) {
  cdouble_t ad(1, 2);
  double bd = std::norm(ad);
  cvar_t a(1, 2);
  var_t b = std::norm(a);
  EXPECT_FLOAT_EQ(bd, b.val());
}
TEST(mathRevCore, stdConj1) {
  cdouble_t ad(1, 2);
  cdouble_t bd = std::conj(ad);
  cvar_t a(1, 2);
  cvar_t b = std::conj(a);
  expect_complex(bd, b);
}
TEST(mathRevCore, stdProj1) {
  double inf = std::numeric_limits<double>::infinity();
  std::vector<double> args{-1, 0, 1, inf, -inf};
  for (double re : args) {
    for (double im : args) {
      cdouble_t ad(re, im);
      cdouble_t bd = std::proj(ad);
      cvar_t a(re, im);
      cvar_t b = std::proj(a);
      expect_complex(bd, b);
    }
  }
}
TEST(mathRevCore, stdPolar1) {
  double r_d = 0.5;
  double theta_d = 1.3;
  cdouble_t a_d = std::polar(r_d, theta_d);

  var_t r = 0.5;
  var_t theta = 1.3;
  cvar_t a = std::polar(r, theta);
  expect_complex(a_d, a);

  double inf = std::numeric_limits<double>::infinity();
  double nan = std::numeric_limits<double>::quiet_NaN();

  // these are undefined behaviors, but we return NaN
  var_t neg1_v = -1;
  var_t one_v = 1;
  var_t two_v = 2;
  var_t nan_v = nan;
  var_t inf_v = inf;
  EXPECT_TRUE(stan::math::is_nan(std::polar(neg1_v, two_v).real()));
  EXPECT_TRUE(stan::math::is_nan(std::polar(nan_v, two_v).real()));
  EXPECT_TRUE(stan::math::is_nan(std::polar(one_v, inf_v).real()));
}
TEST(mathRevCore, stdExp1) {
  expect_complex_common([](const auto& u) { return std::exp(u); });
}
TEST(mathRevCore, stdLog1) {
  expect_complex_common([](const auto& u) { return std::log(u); });
}
TEST(mathRevCore, stdLog101) {
  expect_complex_common([](const auto& u) { return std::log10(u); });
}
TEST(mathRevCore, stdPow1) {
  auto f = [](const auto& u, const auto& v) {
    // using std::pow;
    return pow(u, v);
  };
  expect_complex_common_binary(f);
  // verify (cvar_t, int) is unambiguous
  expect_complex(f(cdouble_t{1.2, 0.3}, 2), f(cvar_t{1.2, 0.3}, 2));
}
TEST(mathRevCore, stdSqrt1) {
  expect_complex_common([](const auto& u) { return std::sqrt(u); });
}
TEST(mathRevCore, stdSinh1) {
  expect_complex_common([](const auto& u) { return std::sinh(u); });
}
TEST(mathRevCore, stdCosh1) {
  expect_complex_common([](const auto& u) { return std::cosh(u); });
}
TEST(mathRevCore, stdTanh1) {
  expect_complex_common([](const auto& u) { return std::tanh(u); });
}
TEST(mathRevCore, stdAsinh1) {
  expect_complex_common([](const auto& u) { return std::asinh(u); });
}
TEST(mathRevCore, stdAcosh1) {
  expect_complex_common([](const auto& u) { return std::acosh(u); });
}
TEST(mathRevCore, stdAtanh1) {
  expect_complex_common([](const auto& u) { return std::atanh(u); });
}
TEST(mathRevCore, stdSin1) {
  expect_complex_common([](const auto& u) { return std::sin(u); });
}
TEST(mathRevCore, stdCos1) {
  expect_complex_common([](const auto& u) { return std::cos(u); });
}
TEST(mathRevCore, stdTan1) {
  expect_complex_common([](const auto& u) { return std::tan(u); });
}
TEST(mathRevCore, stdAsin1) {
  expect_complex_common([](const auto& u) { return std::asin(u); });
}
TEST(mathRevCore, stdAcos1) {
  expect_complex_common([](const auto& u) { return std::acos(u); });
}
TEST(mathRevCore, stdAtan1) {
  expect_complex_common([](const auto& u) { return std::acos(u); });
}
TEST(mathRevCore, eigenSolver) {
  // test adapted from https://github.com/stan-dev/math/pull/789/files
  typedef Eigen::Matrix<stan::math::var, -1, -1> matrix_v_t;
  mvar_t a(3, 3);
  a << 1, 2, 3, 0.7, 0.11, 0.13, -5, -17, -23;
  Eigen::EigenSolver<matrix_v_t> s(a);
  auto ev = s.eigenvectors();
  auto I
      = (ev.inverse() * a * ev * s.eigenvalues().asDiagonal().inverse()).real();
  expect_identity_matrix(I);
}
TEST(mathRevCore, pseudoEigendecomposition) {
  // test adapted from https://github.com/stan-dev/math/pull/789/files
  mvar_t a(3, 3);
  a << 1, 2, 3, 0.7, 0.11, 0.13, -5, -17, -23;
  Eigen::EigenSolver<mvar_t> s(a);
  mvar_t D = s.pseudoEigenvalueMatrix();
  mvar_t V = s.pseudoEigenvectors();
  mvar_t I = V.inverse() * a * V * D.inverse();
  expect_identity_matrix(I);
}
TEST(mathRevCore, complexSchur) {
  // test adapted from https://github.com/stan-dev/math/pull/789/files
  mvar_t a(3, 3);
  a << 1, 2, 3, 0.7, 0.11, 0.13, -5, -17, -23;
  Eigen::ComplexSchur<mvar_t> s(a);
  auto M = (s.matrixU().adjoint() * s.matrixU()).eval();
  mvar_t I = M.real() + M.imag();
  expect_identity_matrix(I);
}