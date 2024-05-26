
#include <iostream>

void foo(int&){std::cout << "foo called" << std::endl;} // expect an lvalue as 1st argument

template <typename T>
void deduce(T&& x)
{
  foo(x);
}

int main()
{
  int i = 2; // `i` is an `lvalue`

  // here x is an `rvalue`. However, following the rules of
  // reference of reference, it results as `lvalue` in foo,
  // and hence will compile. The problem here is that we want
  // to maintain the value category of the parameter.
  deduce(1); 
  deduce(i);

  return 0;
}