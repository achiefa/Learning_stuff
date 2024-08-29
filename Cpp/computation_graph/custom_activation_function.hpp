#include "mat_policies.hpp"
#include <functional>
#include <eigen3/Eigen/Eigen>

#ifndef INCLUDE_CUSTOM_ACTIVATION
#define INCLUDE_CUSTOM_ACTIVATION

/**
 * @brief Silly class for custom activation function at runtime
 * 
 * @tparam MatType 
 * @tparam EvaluationPolicy 
 */

template <typename MatType = Eigen::VectorXd,
          template <typename> class EvaluationPolicy = EigenPolicy>
class CustomActTemplate
{ 
  public:

    CustomActTemplate(std::function<double(const double& x)> func,
                      std::function<double(const double& x)> dfunc)
    : func_(func)
    {}
    
    double Evaluate(const double& x)
    {
      return func_(x);
    }

    MatType Evaluate(const MatType& mat)
    {
      return EvaluationPolicy<MatType>::eval_element_wise(mat, [this](const double& x){ return Evaluate(x); });
    }

    MatType operator()(const MatType& mat)
    {
      return Evaluate(mat);
    }

  private:
  std::function<double(const double& x)> func_;
};
using CustomActivation = CustomActTemplate<>;
#endif