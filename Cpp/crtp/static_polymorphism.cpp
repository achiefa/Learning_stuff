#include <iostream>

// Implementation of compile-time polymorphism
// and pure virtual function
template <typename D>
class B {
  public:
  B() : _i(1) {}
  void f(int i) { derived()->f_impl(i); }
  int get() const { return _i; }

  protected:
  D* derived() { return static_cast<D*>(this); }
  int _i;
};

class D : public B<D> {
  public:
  void f_impl(int i) { _i*=3;}
};

class C : public B<C> {
  public:
  void f_impl(int i) { _i*=4;}
};

/**
 * This function compiles even for parameters of unknown types and it's
 * needed to exploit full (static) polymorphism.
 */ 

template <typename D>
void apply (B<D>* b, int& i) {
  b->f(++i);
}



int main() {

  return 0;
}