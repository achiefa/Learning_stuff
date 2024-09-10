#include <iostream>
#include <cassert>
#include <type_traits>


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
struct DeleteByOperator
{
  void operator()(T* p) const
  {
    delete p;
  }
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


// Template-template parameter is well explained by Jason Turned
// https://www.youtube.com/watch?v=s6Cub7EFLXo
// However, this strategy doesn't work when:
// - The policy is a non-template object
// - The policy template requires more that one argument
template <typename T,
          // Here I don't pass a class, but a template
          template <typename...> class DeletionPolicy = DeleteByOperator>
class SmartPtr
{
  private:
    DeletionPolicy<T> deletion_policy_;
    T* p_;
    static_assert(std::is_same<void, decltype(deletion_policy_(p_))>::value,
                  "Deletion policy must be callable");

  public:
    explicit SmartPtr (T* p = nullptr, 
                      const DeletionPolicy<T>& del_policy = DeletionPolicy<T>())
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
  // Still need to pass the correct template in the argument of the
  // constructor
  SmartPtr<C, DeleteByOperator> p(new C(4), DeleteByOperator<C>());
  return 0;
}