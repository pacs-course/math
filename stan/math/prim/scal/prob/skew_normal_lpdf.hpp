#ifndef STAN_MATH_PRIM_SCAL_PROB_SKEW_NORMAL_LPDF_HPP
#define STAN_MATH_PRIM_SCAL_PROB_SKEW_NORMAL_LPDF_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/scal/err/check_finite.hpp>
#include <stan/math/prim/scal/err/check_not_nan.hpp>
#include <stan/math/prim/scal/err/check_positive.hpp>
#include <stan/math/prim/scal/err/check_consistent_sizes.hpp>
#include <stan/math/prim/scal/fun/size_zero.hpp>
#include <stan/math/prim/scal/fun/erf.hpp>
#include <stan/math/prim/scal/fun/erfc.hpp>
#include <stan/math/prim/scal/fun/value_of.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <cmath>

namespace stan {
namespace math {

template <bool propto, typename T_y, typename T_loc, typename T_scale,
          typename T_shape>
inline auto skew_normal_lpdf(const T_y& y, const T_loc& mu,
                             const T_scale& sigma, const T_shape& alpha) {
  static const char* function = "skew_normal_lpdf";
  using T_partials = partials_return_t<T_y, T_loc, T_scale, T_shape>;

  using std::exp;
  using std::log;

  if (size_zero(y, mu, sigma, alpha)) {
    return T_partials(0.0);
  }

  T_partials logp(0.0);

  check_not_nan(function, "Random variable", y);
  check_finite(function, "Location parameter", mu);
  check_finite(function, "Shape parameter", alpha);
  check_positive(function, "Scale parameter", sigma);
  check_consistent_sizes(function, "Random variable", y, "Location parameter",
                         mu, "Scale parameter", sigma, "Shape paramter", alpha);

  if (!include_summand<propto, T_y, T_loc, T_scale, T_shape>::value) {
    return T_partials(0.0);
  }

  operands_and_partials<T_y, T_loc, T_scale, T_shape> ops_partials(y, mu, sigma,
                                                                   alpha);

  const scalar_seq_view<T_y> y_vec(y);
  const scalar_seq_view<T_loc> mu_vec(mu);
  const scalar_seq_view<T_scale> sigma_vec(sigma);
  const scalar_seq_view<T_shape> alpha_vec(alpha);
  size_t N = max_size(y, mu, sigma, alpha);

  VectorBuilder<true, T_partials, T_scale> inv_sigma(length(sigma));
  VectorBuilder<include_summand<propto, T_scale>::value, T_partials, T_scale>
      log_sigma(length(sigma));
  for (size_t i = 0; i < length(sigma); i++) {
    inv_sigma[i] = 1.0 / value_of(sigma_vec[i]);
    if (include_summand<propto, T_scale>::value) {
      log_sigma[i] = log(value_of(sigma_vec[i]));
    }
  }

  for (size_t n = 0; n < N; n++) {
    const T_partials y_dbl = value_of(y_vec[n]);
    const T_partials mu_dbl = value_of(mu_vec[n]);
    const T_partials sigma_dbl = value_of(sigma_vec[n]);
    const T_partials alpha_dbl = value_of(alpha_vec[n]);

    const T_partials y_minus_mu_over_sigma = (y_dbl - mu_dbl) * inv_sigma[n];
    const double pi_dbl = pi();

    if (include_summand<propto>::value) {
      logp -= 0.5 * log(2.0 * pi_dbl);
    }
    if (include_summand<propto, T_scale>::value) {
      logp -= log(sigma_dbl);
    }
    if (include_summand<propto, T_y, T_loc, T_scale>::value) {
      logp -= y_minus_mu_over_sigma * y_minus_mu_over_sigma / 2.0;
    }
    if (include_summand<propto, T_y, T_loc, T_scale, T_shape>::value) {
      logp += log(erfc(-alpha_dbl * y_minus_mu_over_sigma / std::sqrt(2.0)));
    }

    T_partials deriv_logerf
        = 2.0 / std::sqrt(pi_dbl)
          * exp(-alpha_dbl * y_minus_mu_over_sigma / std::sqrt(2.0) * alpha_dbl
                * y_minus_mu_over_sigma / std::sqrt(2.0))
          / (1 + erf(alpha_dbl * y_minus_mu_over_sigma / std::sqrt(2.0)));
    if (!is_constant_all<T_y>::value) {
      ops_partials.edge1_.partials_[n]
          += -y_minus_mu_over_sigma / sigma_dbl
             + deriv_logerf * alpha_dbl / (sigma_dbl * std::sqrt(2.0));
    }
    if (!is_constant_all<T_loc>::value) {
      ops_partials.edge2_.partials_[n]
          += y_minus_mu_over_sigma / sigma_dbl
             + deriv_logerf * -alpha_dbl / (sigma_dbl * std::sqrt(2.0));
    }
    if (!is_constant_all<T_scale>::value) {
      ops_partials.edge3_.partials_[n]
          += -1.0 / sigma_dbl
             + y_minus_mu_over_sigma * y_minus_mu_over_sigma / sigma_dbl
             - deriv_logerf * y_minus_mu_over_sigma * alpha_dbl
                   / (sigma_dbl * std::sqrt(2.0));
    }
    if (!is_constant_all<T_shape>::value) {
      ops_partials.edge4_.partials_[n]
          += deriv_logerf * y_minus_mu_over_sigma / std::sqrt(2.0);
    }
  }
  return ops_partials.build(logp);
}

template <typename T_y, typename T_loc, typename T_scale, typename T_shape>
inline auto skew_normal_lpdf(const T_y& y, const T_loc& mu,
                             const T_scale& sigma, const T_shape& alpha) {
  return skew_normal_lpdf<false>(y, mu, sigma, alpha);
}

}  // namespace math
}  // namespace stan
#endif
