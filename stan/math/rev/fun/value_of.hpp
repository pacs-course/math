#ifndef STAN_MATH_REV_FUN_VALUE_OF_HPP
#define STAN_MATH_REV_FUN_VALUE_OF_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>

namespace stan {
namespace math {

/**
 * Return the value of the specified variable.
 *
 * <p>This function is used internally by autodiff functions along
 * with <code>value_of(T x)</code> to extract the
 * <code>double</code> value of either a scalar or an autodiff
 * variable.  This function will be called when the argument is a
 * <code>var</code> even if the function is not
 * referred to by namespace because of argument-dependent lookup.
 *
 * @param v Variable.
 * @return Value of variable.
 */
template <typename Var, require_var_t<Var>* = nullptr>
inline decltype(auto) value_of(Var&& v) { return v.vi_->val_; }

template <typename EigMat, require_eigen_t<EigMat>* = nullptr, require_vt_var<EigMat>* = nullptr>
inline auto value_of(EigMat&& x) {
  return x.val().eval();
}


}  // namespace math
}  // namespace stan
#endif
