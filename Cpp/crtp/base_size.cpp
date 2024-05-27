#include <iostream>
#include <vector>

template <typename D>
class B {
  // The template for class B has to instantiate
  // with type `D` which is an incomplete type.
  // This will generate a compile-time error, also
  // known as `incomplete type error`.
  typedef typename D::value_type value_type;
  value_type* p;
};

template <typename C>
class stack {
  C c_;

  public:
  typedef typename C::value_type value_type;
  void push(const value_type& v) { c_.push_back(v); }
  value_type pop() {
    value_type v = c_.back();
    c_.pop_back();
    return v;
  }
};

// The problem lies here, not in B. When we write
//      class D : public B<D>,
// B<D> has to be known, but D has not been defined yet.
// Indeed, D is defined in the incoming lines, but not at the
// point where inheritance takes place. But the declaration
// of class D requires to know the base class B<D>, so that
// it is a vicious circle. TO solve that, D is forward-declared.
// Now, the template for the base class could also be applied to
// a forward-declared type (which is what happens in CRTP (?)),
// which is an incomplete type. The problem being that it is not
// possible to reference to a type declared inside an incomplete
// type, because FORWARD DECLARATION OF A NESTED CLASS IS NOT ALLOWED!
/* class D : public B<D> {
  typedef int value_type;
}; */

/**
 * To solve this problem, we can first note that the body of a member
 * function of a class template (B) is not instantiated until it is
 * called. Given a template parameter D, the member function does not
 * even have to compile, as long as it's not called. Thus, references
 * to the derived (incomplete) class, its nested types and member functions,
 * inside the member function of the base class are perfectly fine!
 * Moreover, inside the base class the derived class is forward-declared
 * and it's still possible to use pointers end references to it. We can
 * then use static cast, as we do in CRTP.
 * The fact that members functions are not compiled until they are
 * called allows us to deduce the `type` if we are happy to define 
 * the type in the member function.
 */
template <typename D>
class B1 {
  public:
  void test () {
    typedef typename D::value_type value_type;
    value_type a;
    std::cout << typeid(a).name() << std::endl;
  }
  //value_type* p;
  D* derived() { return static_cast<D*>(this); }
  D* derived_;
};

class D1 : public B1<D1> {
  public:
  typedef int value_type;
};

class A {
  public:
  typedef int value_type;
  value_type x_;
};


int main() {
  stack<std::vector<int>> s;
  B<A> c;

  B1<D1>* b1 = new D1;
  b1->test();
  return 0;
}