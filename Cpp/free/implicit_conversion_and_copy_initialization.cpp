#include <iostream>

struct A
{
  A(int a) {std::cout << a << std::endl;}
  A(int, int) {}
  operator bool() const { return true; }
};

struct B
{
  explicit B(int) {}
  explicit B(int, int) {}
  explicit operator bool() const { return true; }
};

struct C
{
  explicit C(int&&) {}
  explicit C(int, int) {}
  explicit operator bool() const { return true; }
};

int main()
{ 
  int a_int = 4;
  double a_double = 4.4;
  int l_value = 2;

  // std::move return a `xvalue` reference.
  // `int&&` specifies a `rvalue`, which can be
  // either `prvalue` or `xvalue`.
  // `xvalue` refers to an object, or temporary object
  int&& r_value = std::move(l_value); // equivalent to static_cast<int&&>(l_value)

  A a1 = 1;
  A a2(2);
  A a3(3.4);
  A a4(a_double);
  A a5(a_int);

  //B b1 = 1; // Copy-initialization non allowed
  B b2(2);
  B b3(3.4);
  B b4(a_double);
  B b5(a_int);

  C c2(2);
  C c3(3.4);
  C c4(a_double);
  C c5(static_cast<int&&>(a_int));
  //C c6(l_value);
  //C c7(std::move(l_value));



  return 0;
}