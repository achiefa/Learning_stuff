#include <iostream>
#include <cassert>
#include <type_traits>

struct DeleteByOperator
{ 
  // The member function template is instantiated
  // on the type of object that needs to be deleted.
  // In other words, the compiler uses the `template
  // argument deduction` to deduce the type of the
  // argument.
  template <typename T>
  void operator()(T* p) const
  {
    delete p;
  }
};



template <typename T,
          typename DeletionPolicy = DeleteByOperator>
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
  // Since the delete-policy is stateless, the SmartPtr
  // object can be instantiated as follows
  //SmartPtr<C, DeleteByOperator> p(new C(2), DeleteByOperator()); // before C++17
  SmartPtr<C> p (new C(2), DeleteByOperator()); // C++17
  return 0;
}