#if __cplusplus < 202002L
#error "This example needs C++20"
#else
#include <iostream>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <cstdlib>


class SmallHeap
{
  public:
    SmallHeap() {}
    ~SmallHeap() {}

    void* allocate(size_t s)
    {
      assert(s <= size_);
      return mem_;
    }

    void deallocate(void* p)
    {
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
struct DeleteByOperator
{
  void operator()(T* p) const
  {
    delete p;
  }
};


struct DeleteHeap
{
  explicit DeleteHeap(SmallHeap& heap)
  : heap_(heap) {}

  template <typename T>
  void operator()(T* p) const
  {
    p->~T();
    heap_.deallocate(p);
  }

  private:
    // In this case the object `DeleteHeap` has a
    // state, which is the heap.
    SmallHeap& heap_;
};


template <typename T, typename F>
concept Callable1 = requires(F f, T* p)
{
  { f(p) } -> std::same_as<void>;
};


template <typename T,
          typename DeletionPolicy = DeleteByOperator<T>>
requires Callable1<T, DeletionPolicy>
class SmartPtr
{
  private:
    DeletionPolicy deletion_policy_;
    T* p_;

  public:
    explicit SmartPtr (T* p = nullptr, 
                      const DeletionPolicy& del_policy = DeletionPolicy())
    : p_(p), deletion_policy_(del_policy)
    {/* do nothing */}

    ~SmartPtr() 
    {
      std::cout << "Calling destructor" << std::endl;
      deletion_policy_(p_);
    }

    T* operator->() 
    { 
      std::cout << "Operator `->` w/o const" << std::endl;
      return p_; 
    }

    const T* operator->() const
    { 
      std::cout << "Operator `->` w/ const" << std::endl;
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


int main()
{
  SmallHeap heap;
  SmartPtr p1(new C(4));
  SmartPtr p2(new C(4), DeleteByOperator<C>());
  SmartPtr  p3(new(&heap) C(4), DeleteHeap(heap));

  return 0;
}

#endif