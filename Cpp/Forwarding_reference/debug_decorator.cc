#include <stdlib.h>
#include <string>
#include <iostream>

template <typename Callable>
class DebugDecorator
{
  public:
    DebugDecorator(const Callable& c, const char* s) : c_(c), s_(s) {}

    // Variadic function template
    template <typename ... Args> // template parameter pack -- variadic template
    auto operator() (Args&& ... args) const
    {
      // If args is `lvalue`, then we have a `lvalue` reference to T.
      // If args is `rvalue`, then we have a `rvalue` reference to T.
      std::cout << "Invoking " << s_ << std::endl;
      auto res = c_(std::forward<Args>(args) ...);
      std::cout << "Result: " << res << std::endl;
      return res;
    }

  private:
    const Callable& c_;
    const std::string s_;
};

// The problem with the decorator is that it may be hard to write out the type of the callable
// Let's say we a function f somewhere in the project but we don't know its type (or it is 
// dynamic). Hence we won't be able to deduce it with the following
//
//  DebugDecorator<type_of_f> debug_f (f, 'f')
//
// However, we can use template argument deduction to ask the compiler
// to deduce the type of the passed function
template < typename Callable >
auto decorate_debug(const Callable& c, const char* s)
{
  return DebugDecorator<Callable> (c,s);
}
// This template function deduces the type of the `Callable` and
// decorates it with the debugging wrapper.
// Since C++17, we also have Class Template Argument Deduction (CTAD)
// and we don't need this template function.
int g(int i1, int i2) {return i1 - i2;}

int main()
{
  auto g1 = decorate_debug(g, "g()");
  g1(1,2);

  DebugDecorator g2(g, "g()");
  g2(1,2);


  return 0;
}