#include <eigen3/Eigen/Eigen>
#include <iostream>

/*template <template<typename> typename distribution, typename RNG = std::mt19937, typename ...Args>
requires std::uniform_random_bit_generator<RNG>
void RandomInit(RNG* g, std::tuple<Args...> tuple)
{
  auto Is = std::index_sequence_for<Args...>{};
  auto d = distribution<RNG>(g, get<Is>(tuple)...);
  std::cout << d() << std::endl;
}*/

template <typename Tuple, std::size_t... Is>
void Easy(const Tuple& t,  
          std::index_sequence<Is...>)
{
  (
    (std::cout << (Is ? ", " : "") << std::get<Is>(t))
    , ...);
}

template <typename ...Args>
void printEasy(const std::tuple<Args...>& t)
{
  Easy(t, std::index_sequence_for<Args...>{});
}



int main()
{
  Eigen::MatrixXd mat (5,5);
  mat.setOnes();
  Eigen::VectorXd vec(5);
  vec << 1,2,3,4,5;
  mat.diagonal() = vec;
  std::cout << mat << std::endl;


  return 0;
}