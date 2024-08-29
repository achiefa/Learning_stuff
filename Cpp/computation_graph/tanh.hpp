#include "node.hpp"
#include "mat_policies.hpp"
#include <functional>

#ifndef INCLUDE_TANH
#define INCLUDE_TANH

/**
 * @brief Implementation of the activation layer for tanh
 * 
 * @tparam MatType specifies the matrix type. Default is Eigen matrix
 * @tparam EvaluationPolicy specifies the element-wise operation
 *         according to the matrix type
 * 
 * @details Note that the `EvaluationPolicy` is stored in the class, as it
 *          happens to be a static class.
 * 
 * @todo Is there a way to check whether `EvaluationPolicy` is a static class
 *       with `eval_element_wise` method,
 */
template <typename MatType = Eigen::VectorXd,
          template <typename> class EvaluationPolicy = EigenPolicy>
class TanhTemplate
{ 
  public:
    double Evaluate(const double& x)
    {
      return tanh(x);
    }

    MatType Evaluate(const MatType& mat)
    {
      return EvaluationPolicy<MatType>::eval_element_wise(mat, [this](const double& x){ return Evaluate(x); });
    }

    MatType operator()(const MatType& mat)
    {
      return Evaluate(mat);
    }
};
using Tanh = TanhTemplate<>;
#endif