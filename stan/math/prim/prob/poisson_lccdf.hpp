#ifndef STAN_MATH_PRIM_PROB_POISSON_LCCDF_HPP
#define STAN_MATH_PRIM_PROB_POISSON_LCCDF_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/constants.hpp>
#include <stan/math/prim/fun/exp.hpp>
#include <stan/math/prim/fun/gamma_p.hpp>
#include <stan/math/prim/fun/lgamma.hpp>
#include <stan/math/prim/fun/log.hpp>
#include <stan/math/prim/fun/max_size.hpp>
#include <stan/math/prim/fun/promote_scalar.hpp>
#include <stan/math/prim/fun/size.hpp>
#include <stan/math/prim/fun/size_zero.hpp>
#include <stan/math/prim/fun/to_ref.hpp>
#include <stan/math/prim/fun/value_of.hpp>
#include <stan/math/prim/functor/operands_and_partials.hpp>
#include <cmath>
#include <limits>

namespace stan {
namespace math {

template <typename T_n, typename T_rate>
return_type_t<T_rate> poisson_lccdf(const T_n& n, const T_rate& lambda) {
  using T_partials_return = partials_return_t<T_n, T_rate>;
  using T_n_ref = ref_type_if_t<!is_constant<T_n>::value, T_n>;
  using T_lambda_ref = ref_type_if_t<!is_constant<T_rate>::value, T_rate>;
  static const char* function = "poisson_lccdf";
  check_consistent_sizes(function, "Random variable", n, "Rate parameter",
                         lambda);

  T_n_ref n_ref = n;
  T_lambda_ref lambda_ref = lambda;

  const auto& n_col = as_column_vector_or_scalar(n_ref);
  const auto& lambda_col = as_column_vector_or_scalar(lambda_ref);

  const auto& n_arr = as_array_or_scalar(n_col);
  const auto& lambda_arr = as_array_or_scalar(lambda_col);

  ref_type_t<decltype(value_of(n_arr))> n_val = value_of(n_arr);
  ref_type_t<decltype(value_of(lambda_arr))> lambda_val = value_of(lambda_arr);

  check_nonnegative(function, "Rate parameter", lambda_val);

  if (size_zero(n, lambda)) {
    return 0;
  }

  operands_and_partials<T_lambda_ref> ops_partials(lambda_ref);

  if (sum(promote_scalar<int>(n_val < 0))) {
    return ops_partials.build(0.0);
  }

  const auto& log_Pi = to_ref_if<!is_constant_all<T_rate>::value>(
      log(gamma_p(n_val + 1.0, lambda_val)));
  T_partials_return P = sum(log_Pi);

  if (!is_constant_all<T_rate>::value) {
    ops_partials.edge1_.partials_ = exp(n_val * log(lambda_val) - lambda_val
                                        - lgamma(n_val + 1.0) - log_Pi);
  }

  return ops_partials.build(P);
}

}  // namespace math
}  // namespace stan
#endif
