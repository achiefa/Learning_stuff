
#include <iostream>

void foo(int&){std::cout << "foo called" << std::endl;} // expect an lvalue as 1st argument

template <typename T>
void deduce(T&& x)
{
  foo(static_cast<T&&>(x));
}

template <typename T>
void deduce_forward(T&& x)
{
  foo(std::forward<T>(x));
}

int main()
{
  int i = 2; // `i` is an `lvalue`

  deduce(1); // here x is an `rvalue` and with static_cast it is passed as `rvalue` to foo --> won't compile
  //deduce and deduce_forward are equivalent
  deduce_forward(1);
  deduce(i);

  return 0;
}