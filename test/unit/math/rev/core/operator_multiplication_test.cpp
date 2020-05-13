#include <stan/math/rev/core.hpp>
#include <test/unit/math/rev/fun/util.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <limits>


TEST(MathRev, TestVarEigen) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, -1, -1> x_vals(3, 3);
  Eigen::Matrix<double, -1, -1> y_vals(3, 3);
  x_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  y_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  var_value<Eigen::Matrix<double, -1, -1>> x = x_vals;
  var_value<Eigen::Matrix<double, -1, -1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, -1, -1> x_dyn = x_vals;
  Eigen::Matrix<var, -1, -1> y_dyn = y_vals;
  Eigen::Matrix<var, -1, -1> mul_xy_dyn = multiply(x_dyn, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}



TEST(MathRev, TestVarEigenMatColVec) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, -1, -1> x_vals(3, 3);
  Eigen::Matrix<double, -1, 1> y_vals(3);
  x_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  y_vals << 1, 2, 3;
  var_value<Eigen::Matrix<double, -1, -1>> x = x_vals;
  var_value<Eigen::Matrix<double, -1, 1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, -1, -1> x_dyn = x_vals;
  Eigen::Matrix<var, -1, 1> y_dyn = y_vals;
  Eigen::Matrix<var, -1, 1> mul_xy_dyn = multiply(x_dyn, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}

TEST(MathRev, TestVarEigenRowVecMat) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, 1, -1> x_vals(3);
  Eigen::Matrix<double, -1, -1> y_vals(3, 3);
  x_vals << 1, 2, 3;
  y_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  var_value<Eigen::Matrix<double, 1, -1>> x = x_vals;
  var_value<Eigen::Matrix<double, -1, -1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, 1, -1> x_dyn = x_vals;
  Eigen::Matrix<var, -1, -1> y_dyn = y_vals;
  Eigen::Matrix<var, 1, -1> mul_xy_dyn = multiply(x_dyn, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}

TEST(MathRev, TestVarEigenColVecRowVec) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, 1, -1> x_vals(3);
  Eigen::Matrix<double, -1, 1> y_vals(3);
  x_vals << 1, 2, 3;
  y_vals << 1, 2, 3;
  var_value<Eigen::Matrix<double, 1, -1>> x = x_vals;
  var_value<Eigen::Matrix<double, -1, 1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, 1, -1> x_dyn = x_vals;
  Eigen::Matrix<var, -1, 1> y_dyn = y_vals;
  auto mul_xy_dyn = multiply(x_dyn, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}

TEST(MathRev, TestVarEigenRowVecColVec) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, -1, 1> x_vals(3);
  Eigen::Matrix<double, 1, -1> y_vals(3);
  x_vals << 1, 2, 3;
  y_vals << 1, 2, 3;
  var_value<Eigen::Matrix<double, -1, 1>> x = x_vals;
  var_value<Eigen::Matrix<double, 1, -1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, -1, 1> x_dyn = x_vals;
  Eigen::Matrix<var, 1, -1> y_dyn = y_vals;
  auto mul_xy_dyn = multiply(x_dyn, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}


TEST(MathRev, TestVarEigenVarDbl) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, -1, -1> x_vals(3, 3);
  Eigen::Matrix<double, -1, -1> y_vals(3, 3);
  x_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  y_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  var_value<Eigen::Matrix<double, -1, -1>> x = x_vals;
  var lp = 0;
  auto mul_xy = x * y_vals;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "x stat val: \n" << x.val() << "\n";
  std::cout << "x stat adj: \n" << x.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, -1, -1> x_dyn = x_vals;
  Eigen::Matrix<var, -1, -1> mul_xy_dyn = multiply(x_dyn, y_vals);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "x dyn val: \n" << x_dyn.val() << "\n";
  std::cout << "x dyn adj: \n" << x_dyn.adj() << "\n";
  puts("---------");
  stan::math::recover_memory();
}

TEST(MathRev, TestVarEigenDblVal) {
  using stan::math::var_value;
  using stan::math::var;
  using stan::math::sum;
  Eigen::Matrix<double, -1, -1> x_vals(3, 3);
  Eigen::Matrix<double, -1, -1> y_vals(3, 3);
  x_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  y_vals << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  var_value<Eigen::Matrix<double, -1, -1>> y = y_vals;
  var lp = 0;
  auto mul_xy = x_vals * y;
  auto sum_mul_xy = sum(mul_xy);
  lp -= sum_mul_xy;
  lp.grad();
  puts("-------------");
  std::cout << "lp static val: \n" << lp.val() << "\n";
  std::cout << "lp static adj: \n" << lp.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy static val: \n" << sum_mul_xy.val() << "\n";
  std::cout << "sum_mul_xy static adj: \n" << sum_mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy stat val: \n" << mul_xy.val() << "\n";
  std::cout << "mul_xy stat adj: \n" << mul_xy.adj() << "\n";
  puts("---------");

  std::cout << "y stat val: \n" << y.val() << "\n";
  std::cout << "y stat adj: \n" << y.adj() << "\n";

  stan::math::set_zero_all_adjoints();
  puts("");
  puts("---------");
  puts("Dynamic Matrix");
  puts("---------");
  puts("");
  Eigen::Matrix<var, -1, -1> y_dyn = y_vals;
  Eigen::Matrix<var, -1, -1> mul_xy_dyn = multiply(x_vals, y_dyn);
  var lp_dyn = 0;
  var sum_mul_xy_dyn = sum(mul_xy_dyn);
  lp_dyn -= sum_mul_xy_dyn;
  lp_dyn.grad();
  std::cout << "lp dyn val: \n" << lp_dyn.val() << "\n";
  std::cout << "lp dyn adj: \n" << lp_dyn.adj() << "\n";
  puts("---------");
  std::cout << "sum_mul_xy dyn val: \n" << sum_mul_xy_dyn.val() << "\n";
  std::cout << "sum_mul_xy dyn adj: \n" << sum_mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "mul_xy dyn val: \n" << mul_xy_dyn.val() << "\n";
  std::cout << "mul_xy dyn adj: \n" << mul_xy_dyn.adj() << "\n";
  puts("---------");

  std::cout << "y dyn val: \n" << y_dyn.val() << "\n";
  std::cout << "y dyn adj: \n" << y_dyn.adj() << "\n";
  stan::math::recover_memory();
}