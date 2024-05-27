#include <stdlib.h>
#include <iostream>

template <typename D>
class registry {
  public:
  static size_t count;
  static D* head;
  D* prev;
  D* next;

  protected:
  registry() {
    //std::cout << "Registry called" << std::endl;
    ++count;
    prev = nullptr;
    next = head;
    head = derived();
    if (next) next->prev = head;
  }
  registry(const registry&) {
    ++count;
    prev = nullptr;
    next = head;
    head = derived();
    if (next) next->prev = head;
  }
  ~registry() {
    --count;
    if (prev) prev->next = next;
    if (next) next->prev = prev;
    if (head == this) head = next;
  }

  private:
  D* derived() { return static_cast<D*>(this); }
};

template <typename D> size_t registry<D>::count(0);
template <typename D> D* registry<D>::head(nullptr);

class base1 : public registry<base1> {
  friend std::ostream& operator<<(std::ostream& out, const base1& a) { out << "I'm base1"; return out; }
  public:
  virtual ~base1 () {}
  virtual void print() = 0;
};

class base2 : public registry<base2> {
  friend std::ostream& operator<<(std::ostream& out, const base2& a) { out << a._i; return out; }
  public:
  base2(int i) : _i(i) {} 
  virtual ~base2 () {}
  virtual void print() = 0;
  
  protected:
  int _i;
};

class derived1 : public base1 {
  public:
  void print () { std::cout << "I am derived1" << std::endl; }
};

class derived2 : public base2 {
  public:
  derived2 (int i) : base2(i) {}
  void print () { std::cout << "I am derived2" << std::endl; }
};

template <typename T>
void report() {
  std::cout << "Total Count: " << T::count << std::endl;
  for (const T* p = T::head; p; p = p->next) {
    std::cout << " " << *p;
  }
  std::cout << std::endl;
}

class A : public registry<A> {
    int i_;
    public:
    A(int i) : i_(i) {}
    friend std::ostream& operator<<(std::ostream& out, const A& a) { out << a.i_; return out; }
};

class B : public registry<B> {
    int i_;
    public:
    B(int i) : i_(i) {}
    friend std::ostream& operator<<(std::ostream& out, const B& b) { out << b.i_; return out; }
};


int main(){
  {
    derived1 d1{};
    std::cout << base1::count << " " << base2::count << std::endl;
  }
  std::cout << base1::count << " " << base2::count << std::endl;
  {
    derived2 d2{1};
    derived2 d2_m = std::move(d2);
    std::cout << base1::count << " " << base2::count << std::endl;
  }
  std::cout << base1::count << " " << base2::count << std::endl;
  {
    std::vector<derived2> v { 1,2,3,4 };
    std::cout << base1::count << " " << base2::count << std::endl;
  }

  {
    A a(0);
    std::cout << A::count << " " << B::count << std::endl;
  }
  std::cout << A::count << " " << B::count << std::endl;
  {
    B b1(1);
    B b2 = std::move(b1);
    std::cout << A::count << " " << B::count << std::endl;
  }
  std::cout << A::count << " " << B::count << std::endl;

  std::cout << "I'm here" << std::endl;
  report<A>();
  A* a4 = nullptr;
  {
    A a5(5);
    report<A>();
    a4 = new A(4);
    report<A>();
    A a1(1);
    report<A>();
    A a2(2);
    report<A>();
  }
  report<A>();

  delete a4;
  return 0;
}