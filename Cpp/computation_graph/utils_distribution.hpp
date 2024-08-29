#include <random>
#ifndef INCLUDE_UTILS_DISTRIBUTION
#define INCLUDE_UTILS_DISTRIBUTION

// The type of RNG is deduced from the argument list through template argument deduction!
template <template<typename> typename distribution, typename RNG = std::mt19937, typename ...Args>
requires std::uniform_random_bit_generator<RNG>
Eigen::CwiseNullaryOp<distribution<RNG>, typename Eigen::MatrixXd>
RandomInit(size_t row, size_t col, RNG* g, Args&& ...args)
{
  return Eigen::MatrixXd::NullaryExpr(row, col, distribution<RNG>(g, args...));
}

#endif