#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <iostream>

int main() {

  Eigen::Tensor<int,3> A (2,2,2);
  Eigen::Tensor<int,3> B (2,2,2);
  A.setValues( { { {1,2} , {3,4}} , { {5,6} , {7,8}} } );
  B.setValues( { { {1,2} , {3,4}} , { {5,6} , {7,8}} } );
  Eigen::array<bool, 2> reverse({true, false});
  Eigen::array<int, 3> transpose({0,2,1});
  std::cout << A + A.shuffle(transpose)  << std::endl;
  return 0;
}