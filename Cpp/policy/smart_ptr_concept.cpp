#if __cplusplus < 202002L
#error "This example needs C++20"
#else
#include <iostream>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <cstdlib>



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


struct A
{
  A(int i)
  : i_(i)
  {}

  int i_;
};


int main()
{
  SmartPtr<A, DeleteByOperator<A>> p (new A(2));
  SmartPtr p_ctads (new A(1)); // Constructor template argument deduction (CTAD)
  
  int i = p->i_;

  const SmartPtr p_const (new A(1));
  int i_const = p_const->i_;

  // WrongDeleteByOperator does not provide the call operator
  //SmartPtr<A, WrongDeleteByOperator2<A>> p_wrong (new A(3));

  std::cout << "Deleting objects..." << std::endl;
  return 0;
}

#endif