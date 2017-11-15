#ifndef STAN_MATH_PRIM_MAT_FUN_LOG_MIX_HPP
#define STAN_MATH_PRIM_MAT_FUN_LOG_MIX_HPP

#include <stan/math/prim/scal/err/check_bounded.hpp>
#include <stan/math/prim/scal/err/check_not_nan.hpp>
#include <stan/math/prim/scal/err/check_consistent_sizes.hpp>
#include <stan/math/prim/scal/err/check_finite.hpp>
#include <stan/math/prim/scal/meta/partials_return_type.hpp>
#include <stan/math/prim/scal/meta/operands_and_partials.hpp>
#include <stan/math/prim/scal/meta/scalar_seq_view.hpp>
#include <stan/math/prim/scal/meta/return_type.hpp>
#include <stan/math/prim/scal/meta/is_constant_struct.hpp>
#include <stan/math/prim/scal/fun/value_of.hpp>
#include <stan/math/prim/mat/meta/length.hpp>
#include <stan/math/prim/mat/fun/log_sum_exp.hpp>
#include <stan/math/prim/mat/fun/log.hpp>

namespace stan {
  namespace math {

    /**
     * Return the log mixture density with specified mixing proportion
     * and log densities.
     *
     * \f[
     * \frac{\partial }{\partial p_x}
     * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
     * \exp^{\log\left(p_n\right)+d_n}\right)
     * =\frac{e^{d_x}}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}
     * \f]
     *
     * \f[
     * \frac{\partial }{\partial d_x}
     * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
     * \exp^{\log\left(p_n\right)+d_n}\right)
     * =\frac{e^{d_x}p_x}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}
     * \f]
     *
     * @param theta vector of mixing proportions in [0, 1].
     * @param lambda vector of log densities.
     * @return log mixture of densities in specified proportion
     */
    template <typename T_theta, typename T_lam>
    typename return_type<T_theta, T_lam>::type
    log_mix(const T_theta& theta, const T_lam& lambda) {
      //static const char* function = "log_mix";
      typedef typename stan::partials_return_type<T_theta, T_lam>::type
        T_partials_return;

      //check_bounded(function, "theta", theta, 0, 1);
      //check_not_nan(function, "lambda", lambda);
      //check_not_nan(function, "theta", theta);
      //check_finite(function, "lambda", lambda);
      //check_finite(function, "theta", theta);
      //check_consistent_sizes(function, "theta", theta, "lambda", lambda);

      const int N = length(theta);

      Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1> theta_dbl(N, 1);
      scalar_seq_view<T_theta> theta_vec(theta);
      for (int n = 0; n < N; ++n)
        theta_dbl[n] = value_of(theta_vec[n]);

      Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1> lam_dbl(N, 1);
      scalar_seq_view<T_lam> lam_vec(lambda);
      for (int n = 0; n < N; ++n)
        lam_dbl[n] = value_of(lam_vec[n]);

      Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1> logp_tmp(N, 1);
      logp_tmp = log(theta_dbl) + lam_dbl;

      T_partials_return logp = log_sum_exp(logp_tmp);

      Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1> theta_deriv(N, 1);
        theta_deriv.array() = (lam_dbl.array() - logp).exp();

      Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1> lam_deriv(N, 1);
        lam_deriv.array() = theta_deriv.array() * theta_dbl.array();

      operands_and_partials<T_theta, T_lam> ops_partials(theta, lambda);
        if (!is_constant_struct<T_theta>::value)
          ops_partials.edge1_.partials_ = theta_deriv;

        if (!is_constant_struct<T_lam>::value)
          ops_partials.edge2_.partials_ = lam_deriv;

      return ops_partials.build(logp);
    }
  }
}
#endif
