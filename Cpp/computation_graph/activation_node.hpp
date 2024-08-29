#include "./node.hpp"

#ifndef INCLUDE_ACTIVATION_NODE
#define INCLUDE_ACTIVATION_NODE

template <typename Func, typename Arg>
concept Callable = requires(Func f, Arg arg) 
  { { f(arg) } -> std::same_as<Arg>; };

template <typename ActivationPolicy>
requires Callable <ActivationPolicy, Eigen::VectorXd>
class Activation : public Node
{ 
  public:
    Activation(VecDep&& dependencies, 
               std::string&& Id,
               const ActivationPolicy& act_policy = ActivationPolicy())
    : act_policy_(act_policy),
      Node(std::forward<VecDep>(dependencies), std::forward<std::string>(Id))
    { /* do nothing */}

    ~Activation() {}
    
    virtual void forward() override
    {
      auto input = dependencies_[0]->GetValue();
      value_ =  act_policy_(input); 
    }


    virtual void dependency_rule() override {}

    virtual void gradient(Eigen::Block<Eigen::MatrixXd,-1,-1>&& g) override {}

    virtual void backward() override {}

  private:
    ActivationPolicy act_policy_;
};
#endif