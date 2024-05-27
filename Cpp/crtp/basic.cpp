#include <iostream>

// Base class is now a class template
// - Base class has compile-time information about the derived class
template <typename D>
class B {
  public:
  B() : _i(1) {
    std::cout << "Base constructed" << std::endl;
  }
  void f(int i) { static_cast<D*>(this)->f(i); }
  int get() const { return _i; }

  ~B() {
    std::cout << "Base destructed" << std::endl;
  }

  protected:
  int _i;
};

// This base class template shows how to consolidate the
// static cast in one place only
template <typename D>
class B2 {
  public:
  B2() : _i(1) {
    std::cout << "Base constructed" << std::endl;
  }
  void f(int i) { derived()->f(i); }
  D* derived() { return static_cast<D*>(this); }
  int get() const { return _i; }

  ~B2() {
    std::cout << "Base destructed" << std::endl;
  }

  protected:
  int _i;
};

class D : public B<D> {
  public:
  D() {
    std::cout << "Derived cosntructed" << std::endl;
  }
  void f(int i) { _i*=3;}
  ~D() {
    std::cout << "Derived destructed" << std::endl;
  }
};



int main() {
  // The call can be done on the base class pointer
  // Indeed, the Type of the class D is B<D> (recursion :) )
  // This will create the object on the heap, but introduces
  // a problem.
  // The problem actually shows up only if we need to delete
  // the object through the base class pointer. When we call
  // `delete`, only the destructor of the base class B<D> is
  // called, but not the one for D.
  B<D>* b = new D; 

  // Of course we can still create the object on the stack 
  // as follows. In this case the object is created with its
  // actual type known, so it is deleted (i.e. when it goes
  // out of scope) the question "What is the type of the deleted
  // object?" is never really asked. So in this case, no polymorphism
  // is involved
  D d;

  return 0;
}