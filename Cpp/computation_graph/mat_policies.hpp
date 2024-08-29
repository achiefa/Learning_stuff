#include <functional>

#ifndef INCLUDE_MAT_POLICIES
#define INCLUDE_MAT_POLICIES

/**
 * @brief Policy for element-wise matrix operations for Eigen objects
 * 
 * @tparam BaseEigen 
 */
template <typename BaseEigen>
struct EigenPolicy
{
  static BaseEigen eval_element_wise(const BaseEigen& mat, 
                       std::function<double(const double&)> func)
  {
    return mat.unaryExpr(func);
  }
};
#endif