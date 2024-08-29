#include "nodes.hpp"


class my_rng{};

int main()
{
  Eigen::MatrixXd mat;
  //std::default_random_engine g_1;
  //Generator generator = g_1;
  size_t row = 5;
  size_t col = 5;
  std::mt19937 g1(10);
  std::mt19937 g2(10);


  //mat = RandomInit<Gaussian>(row, col, &g_mt19937, 10., 0.5);
  //std::cout << mat << std::endl;

  std::normal_distribution d (0.0,1.0);
  std::cout << d(g1) << std::endl;
  std::cout << d(g1) << std::endl;
  std::cout << d(g1) << std::endl;
  std::cout << d(g1) << std::endl;
  std::cout << d(g1) << std::endl;
  std::cout << d(g1) << std::endl;
  std::cout << "______________" << std::endl;
  std::cout << d(g2) << std::endl;
  std::cout << d(g2) << std::endl;
  std::cout << d(g2) << std::endl;
  std::cout << d(g2) << std::endl;
  std::cout << d(g2) << std::endl;
  std::cout << d(g2) << std::endl;

  return 0;
}