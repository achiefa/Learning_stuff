#include <iostream>
#include <cassert>
#include <type_traits>

template <typename T>
struct DeleteByOperator
{
  void operator()(T* p) const
  {
    delete p;
  }
};

template <typename T>
struct DeleteByFree
{
  void operator()(T* p) const
  {
    p->~T();
    free(p);
  }
};

template <typename T>
struct DeleteDestructorOnly
{
  void operator()(T* p) const
  {
    p->~T();
  }
};

template <typename T>
struct WrongDeleteByOperator
{
  void operator[](T* p) const
  {
    delete p;
  }
};

template <typename T>
struct WrongDeleteByOperator2
{
  double operator()(T* p) const
  {
    return 3.4;
  }
};


class SmallHeap
{
  public:
    SmallHeap() {}
    ~SmallHeap() {}

    void* allocate(size_t s)
    {
      assert(s <= size_);
      std::cout << "Allocating new memory on small heap" << std::endl;
      return mem_;
    }

    void deallocate(void* p)
    {
      std::cout << "Deallocating new memory on small heap" << std::endl;
      assert(p == mem_);
    }


  private:
    static constexpr size_t size_ = 1024;
    char mem_[size_];
    SmallHeap(const SmallHeap&) = delete; // Disable copy-constructor
    SmallHeap& operator=(const SmallHeap&) = delete; // Disable assignment operator
};
void* operator new(size_t s, SmallHeap* h) { return h->allocate(s); }



template <typename T>
struct DeleteSmallHeap2
{
  explicit DeleteSmallHeap2(SmallHeap& heap)
  : heap_(heap) {}

  void operator()(T* p) const
  {
    p->~T();
    //heap_.deallocate(p);
  }

  private:
    SmallHeap& heap_;
};


template <typename T>
struct DeleteSmallHeap
{
  explicit DeleteSmallHeap(SmallHeap& heap)
  : heap_(heap) {}

  void operator()(T* p) const
  {
    p->~T();
    heap_.deallocate(p);
  }

  private:
    SmallHeap& heap_;
};

template <typename T>
void delete_T(T* p) { delete p; }



template <typename T,
          typename DeletionPolicy = DeleteByOperator<T>>
class SmartPtr
{
  private:
    DeletionPolicy deletion_policy_;
    T* p_;
    static_assert(std::is_same<void, decltype(deletion_policy_(p_))>::value,
                  "Deletion policy must be callable");

  public:
    explicit SmartPtr (T* p = nullptr, 
                      const DeletionPolicy& del_policy = DeletionPolicy())
    : p_(p), deletion_policy_(del_policy)
    {/* do nothing */}

    ~SmartPtr() 
    {
      std::cout << "Calling destructor..." << std::endl;
      deletion_policy_(p_);
    }

    T* operator->() 
    { 
      //std::cout << "Operator `->` w/o const" << std::endl;
      return p_; 
    }

    const T* operator->() const
    { 
      //std::cout << "Operator `->` w/ const" << std::endl;
      return p_;
    }


    T& operator*() { return *p_; }
    const T& operator*() const { return *p_; }

    
    // copy constructor
    SmartPtr(const SmartPtr&) = delete;
    
    // Assignment operator
    SmartPtr operator=(const SmartPtr&) = delete;

    // Move constructor
    SmartPtr(SmartPtr&& that)
    : p_(std::exchange(that.p_, nullptr)),
      deletion_policy_(std::move(that.deletion_policy_))
    { /* do nothing */}

    // Move assignment
    SmartPtr operator=(SmartPtr&& that)
    {
      deletion_policy_(p_);
      p_ = std::exchange(that.p_, nullptr);
      deletion_policy_ = std::move(that.deletion_policy_);
    }
};


struct A
{
  A(int i)
  : i_(i)
  {}

  int i_;
};

class C
{
  int i_{};

  public:
    explicit C(int i) 
    : i_(i)
    {
      std::cout << "Constructing C(" << i << ") @";
      std::cout << this  << std::endl;
    }

    ~C() { std::cout << "Destroying C @" << this << std::endl;}
    int get() { return i_; }
};

using delete_C_t = void (*)(C*);
void delete_C(C* p) { delete p; }

int main()
{
  //SmartPtr<A, DeleteByOperator<A>> p (new A(2));
  //SmartPtr p_ctads (new A(1)); // Constructor template argument deduction (CTAD)
  
  //int i = p->i_;

  //const SmartPtr p_const (new A(1));
  //int i_const = p_const->i_;

  // WrongDeleteByOperator does not provide the call operator
  //SmartPtr<A, WrongDeleteByOperator2<A>> p_wrong (new A(3));

  //std::cout << "Deleting objects..." << std::endl;

  {
    SmartPtr<C> c (new C(42));
    std::cout << "C: " << c->get() << " @ " << &*c << std::endl;
  }
  {
    std::cout << std::endl;
    SmallHeap h;
    SmartPtr<C, DeleteSmallHeap<C>> c{new(&h) C(24), DeleteSmallHeap<C>(h)};
  }
  {
    //SmartPtr<int, delete_C_t> c(new int(42));
  }
  return 0;
}