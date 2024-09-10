#include <iostream>
#include <functional>

// This struct inherits from std::function
// If the using-declaration refers to a constructor of a direct 
// base of the class being defined, all 
// constructors of that base (ignoring member access) are made 
// visible to overload resolution when initializing the derived class.
//
// For instance, sink<int> just a function that takes an int and returns void.
template<class...Ts> // Variadic template
struct sink : std::function<void(Ts...)> {
  using std::function<void(Ts...)>::function;
};

template<class...Ts>
using source=sink<sink<Ts...>>;

// This thing is a function that takes a sink<char> wrapped
// in a sink object. In doing so, it can play the role of a
// a sink object for another source
source<char> text = [ptr="hello world"]( sink<char> s ){
  for (auto it = ptr; *it; ++it ){ s(*it); }
};

// is equivalent to 
// sink<sink<char>> text = ... which must is a std::function that takes sink<char> and returns void
// sink<char> is std::function<void(char)>, which takes a char and returns void.

template<class In, class Out>
using process=sink< source<In>, sink<Out> >;

//-----------------------
// Stupid test
//-----------------------
template<class...Args>
struct Test {
  Test(Args&&...args)
  {
    ([&] {
      std::cout << args << std::endl;
    } () , ...);  
  }
};

// ------------------------
// Simplified version
//-------------------------

template<class C>
struct base {
  base(C a) : _internal(a) {};
  C _internal;
};

template <class C>
using using_der = base<base<C>>;

using_der<int> boh(1);
//------------------------------

int main(){
  //sink test;
  //Test a(1,2,3);

  auto b = boh._internal;
  std::cout << typeid(b).name() << std::endl;
  std::cout << typeid(b._internal).name() << std::endl;
  std::cout << b._internal << std::endl;

  sink<char> print = [](char c) {std::cout << c;};
  source a = text(print);
  return 0;
}





