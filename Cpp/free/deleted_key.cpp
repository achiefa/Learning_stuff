#include <iostream>

struct T
{ 
  T (int i) : attribute_(i) {}

  void ProintAttribute() { std::cout << attribute_ << std::endl; }
  void operator()() = delete;

  int attribute_;
};


int main()
{
  T t(1);
  t.ProintAttribute();
  // t();    doesn't compile

  return 0;
}