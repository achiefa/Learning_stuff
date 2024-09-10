#include <functional>
#include <memory>
#include <vector>

template <typename FunctionType>
class Node
{
  public:
    using NodePtr = std::shared_ptr<const Node>;

    Node(FunctionType func, std::vector<NodePtr> dependencies)
    : func_(func), dependencies_(std::move(dependencies))
    { /* do nothing */ }
    
    // Perfectly forwarding the result of func_ (whether it is by reference or
    // by value)
    decltype(auto) evaluate() const
    { 

      return func_(depValues);
    }

  private:
    FunctionType func_;
    std::vector<NodePtr> dependencies_;

    template <typename... TupleTypes>
    TupleTypes ArgsTuple(std::vector<NodePtr> dependencies) const
    {
      
    }
};
