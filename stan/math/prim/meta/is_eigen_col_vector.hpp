#ifndef STAN_MATH_PRIM_META_IS_EIGEN_COL_VECTOR_HPP
#define STAN_MATH_PRIM_META_IS_EIGEN_COL_VECTOR_HPP

#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/meta/bool_constant.hpp>
#include <stan/math/prim/meta/is_eigen_dense.hpp>
#include <type_traits>
#include <vector>

namespace stan {
namespace internal {
/** \addtogroup type_trait
 *  @{
 */
/**
 * Underlying implimentation for detecting if an Eigen Matrix is a column
 * vector.
 */
template <typename T, bool = stan::is_eigen_dense<T>::value>
struct is_eigen_col_vector_impl
    : bool_constant<std::decay_t<T>::ColsAtCompileTime == 1> {};

/**
 * Specialization for when type is not an eigen vector.
 */
template <typename T>
struct is_eigen_col_vector_impl<T, false> : std::false_type {};

}  // namespace internal

/**
 * If the input type T is an eigen matrix with 1 row at compile time this
 * has a static member with a value of true. Else this has a static
 * member with a value of false.
 * @tparam T Type to check if it is derived from `EigenBase`
 */
template <typename T>
struct is_eigen_col_vector : internal::is_eigen_col_vector_impl<T> {};
/** @}*/

STAN_ADD_REQUIRE_UNARY(eigen_col_vector, is_eigen_col_vector,
                       require_eigens_types);
STAN_ADD_REQUIRE_CONTAINER(eigen_col_vector, is_eigen_col_vector,
                           require_eigens_types);

}  // namespace stan

#endif
