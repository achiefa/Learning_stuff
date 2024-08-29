#include "node.hpp"

#ifndef INCLUDE_IDENTITY
#define INCLUDE_IDENTITY

template <typename MatType = Eigen::VectorXd>
class IdentityTemplate
{ 
  public:    
    double Evaluate(const double& x)
    {
      return x;
    }

    MatType Evaluate(const MatType& mat)
    {
      return mat;
    }

    MatType operator()(const MatType& mat)
    {
      return Evaluate(mat);
    }
};
using Identity = IdentityTemplate<>;
#endif