#include <random>
#include <eigen3/Eigen/Eigen>
#ifndef INCLUDE_UTILS_DISTRIBUTION
#define INCLUDE_UTILS_DISTRIBUTION


// This function is needed in order to unpack the tuple 
// of parameters for the constructors of distributions.
// The constructor of distributions takes loose parameters
// and not tuple.
// TODO since unpacking a tuple is a general procedure, it this function
// might be generalised to any case. This means that it should not require
// the RNG type.
// TODO Choose better name
template <typename ReturnType, typename RNG, typename Tuple, std::size_t... Is>
ReturnType unpack_tuple(RNG* g, const Tuple& t, std::index_sequence<Is...>)
{
  return ReturnType(g, std::get<Is>(t)...);
}


// The type of RNG is deduced from the argument list through template argument deduction!
template <template<typename> typename distribution, typename RNG = std::mt19937, typename ...Args>
requires std::uniform_random_bit_generator<RNG>
Eigen::CwiseNullaryOp<distribution<RNG>, typename Eigen::MatrixXd>
RandomInit(size_t row, size_t col, RNG* g, const std::tuple<Args...>& tuple)
{
  return Eigen::MatrixXd::NullaryExpr(row, col, unpack_tuple<distribution<RNG>>(g, tuple, std::index_sequence_for<Args...>{}));
}

// TODO This function is specialises to eigen vectors. Maybe it is possible to use a unique
// template for both functions.
template <template<typename> typename distribution, typename RNG = std::mt19937, typename ...Args>
requires std::uniform_random_bit_generator<RNG>
Eigen::CwiseNullaryOp<distribution<RNG>, typename Eigen::VectorXd>
RandomInit(size_t col, RNG* g, const std::tuple<Args...>& tuple)
{
  return Eigen::VectorXd::NullaryExpr(col, unpack_tuple<distribution<RNG>>(g, tuple, std::index_sequence_for<Args...>{}));
}


#endif