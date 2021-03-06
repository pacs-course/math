#ifndef STAN_MATH_REV_FUN_SQUARE_HPP
#define STAN_MATH_REV_FUN_SQUARE_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>

namespace stan {
namespace math {

namespace internal {
class square_vari : public op_v_vari {
 public:
  explicit square_vari(vari* avi) : op_v_vari(avi->val_ * avi->val_, avi) {}
  void chain() { avi_->adj_ += adj_ * 2.0 * avi_->val_; }
};
}  // namespace internal

/**
 * Return the square of the input variable.
 *
 * <p>Using <code>square(x)</code> is more efficient
 * than using <code>x * x</code>.
 *
   \f[
   \mbox{square}(x) =
   \begin{cases}
     x^2 & \mbox{if } -\infty\leq x \leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN}
   \end{cases}
   \f]

   \f[
   \frac{\partial\, \mbox{square}(x)}{\partial x} =
   \begin{cases}
     2x & \mbox{if } -\infty\leq x\leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN}
   \end{cases}
   \f]
 *
 * @param x Variable to square.
 * @return Square of variable.
 */
inline var square(const var& x) {
  return var(new internal::square_vari(x.vi_));
}

/**
 * Return the elementwise square of x
 *
 * @tparam T type of x
 * @param x argument
 * @return elementwise square of x
 */
template <typename T, require_var_matrix_t<T>* = nullptr>
inline auto square(const T& x) {
  return make_callback_var(
      (x.val().array() * x.val().array()).matrix(),
      [x](const auto& vi) mutable {
        x.adj() += (2.0 * x.val().array() * vi.adj().array()).matrix();
      });
}

}  // namespace math
}  // namespace stan
#endif
