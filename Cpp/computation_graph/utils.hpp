#ifndef INCLUDE_UTILS
#define INCLUDE_UTILS

// Concept used in the activation node, and requires 
// the implementation of the activation class to be
// callable
// TODO requires also the `Derive` method
template <typename Func, typename Arg>
concept Callable = requires(Func f, Arg arg) 
  { { f(arg) } -> std::same_as<Arg>; };


// Implementation details
class recursive_functor {
  const Eigen::VectorXd& m_vec;
public:
  recursive_functor(const Eigen::VectorXd& arg) : m_vec(arg) {}
 
  const typename Eigen::VectorXd::Scalar& operator() (Eigen::Index row, Eigen::Index col) const {
    Eigen::Index interval = col - row * m_vec.size();
    if (interval >= 0 && interval < m_vec.size()){
      return m_vec(col % m_vec.size());
    }
    else {
      static const double return_null = 0;
      return return_null;
    }
  }
};


Eigen::CwiseNullaryOp<recursive_functor, typename Eigen::MatrixXd>
makeRecursive(const Eigen::VectorXd& arg1, Eigen::Index rows, Eigen::Index cols)
{
  return Eigen::MatrixXd::NullaryExpr(rows, cols, recursive_functor(arg1.derived()));
}
#endif