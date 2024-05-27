#include <stdlib.h>
#include <iostream>

template <typename D>
class B {
  public:
  B() {
    std::cout << "Base created" << std::endl;
  }

  void destroy(B<D>* b) {
    std::cout << "Derived destructed" << std::endl;
    delete static_cast<D*>(b);
  }

  ~B() {
    std::cout << "Base destructed" << std::endl;
  }
  static size_t count;
};

class D : public B<D> {
  public:
  ~D() {
    std::cout << "Derived destructed internally" << std::endl;
    this->destroy(this);
  }
};

int main(){

  B<D>* b = new D;
  b->destroy(b);
  return 0;
}