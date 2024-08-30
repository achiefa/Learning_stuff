#include "./node.hpp"
#include "utils.hpp"

#ifndef INCLUDE_ACTIVATION_NODE
#define INCLUDE_ACTIVATION_NODE

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
    { 
      outSize_ = inSize_ = dependencies_[0]->GetOutSize();
    }

    ~Activation() {}
    
    virtual void forward() override
    {
      auto input = dependencies_[0]->GetValue();
      value_ =  act_policy_(input); 
    }


    virtual void dependency_rule() override {}

    virtual void gradient(Eigen::Block<Eigen::MatrixXd,-1,-1>&& g) override {}

    virtual void backward() override 
    {
      Eigen::MatrixXd TemporaryMat (outSize_, outSize_);
      TemporaryMat.diagonal() = act_policy_.Derive(dependencies_[0]->GetValue());
      auto test = dependencies_[0]->GetGradient()  + gradient_ * TemporaryMat;
      dependencies_[0]->SetGradient(dependencies_[0]->GetGradient()  + gradient_ * TemporaryMat);
    }

    void initialise_gradients(size_t row)
    { 
      SetGradient(Eigen::MatrixXd::Zero(row, outSize_));
    }
  private:
    ActivationPolicy act_policy_;
};
#endif