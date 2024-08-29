#include "./node.hpp"
#include "distribution_policies.hpp"
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#ifndef INCLUDE_LINEAR_NODE
#define INCLUDE_LINEAR_NODE

/**
 * @todo
 * - Custom initialisation is missing
 * - Backward is still missing
 * - Gradient is still missing
 * - copy constructor
 * - move constructor
 * - assignment operator
 * - ...
 * 
 */

// Implementation details
class recursive_functor {
  const Eigen::VectorXd& m_vec;
public:
  recursive_functor(const Eigen::VectorXd& arg) : m_vec(arg) {}
 
  const typename Eigen::VectorXd::Scalar& operator() (Eigen::Index row, Eigen::Index col) const {
    Eigen::Index interval = col - row * m_vec.size();
    if (interval >= 0 && interval < m_vec.size()){
      return m_vec(col % m_vec.size());
    }
    else {
      static const double return_null = 0;
      return return_null;
    }
  }
};


Eigen::CwiseNullaryOp<recursive_functor, typename Eigen::MatrixXd>
makeRecursive(const Eigen::VectorXd& arg1, Eigen::Index rows, Eigen::Index cols)
{
  return Eigen::MatrixXd::NullaryExpr(rows, cols, recursive_functor(arg1.derived()));
}


template <typename InitDistribution>
class Linear : public Node
{ 
  public:
    Linear(size_t&& size, 
           VecDep&& dependencies, 
           std::string&& Id,
           const InitDistribution& distribution = InitDistribution())
    : InitDistribution_(distribution),
      Node(std::forward<VecDep>(dependencies), std::forward<std::string>(Id))
    {
      outSize_ = size;
      if (!dependencies_.empty()) {
        inSize_ = dependencies_[0]->GetOutSize();
      }
      numberOfParameters_ = outSize_ * (inSize_ + 1);
    }

    virtual ~Linear() {}
    
    virtual void forward() override
    {
      auto input = dependencies_[0]->GetValue();
      value_ =  biases_ + weights_ * input; // MLP architecture
    }


    void dependency_rule() override {
      spdlog::warn("Dependency rule called for {0}, but not yet implemented", Id_);
    }

    virtual void backward() override 
    { 
      // I need to use `SetGradient` and `GetGradient` because the getter method is const.
      dependencies_[0]->SetGradient(dependencies_[0]->GetGradient()  + gradient_ * weights_);
    }

    // TODO: this part can be optimised.
    // TODO: The Eigen::Block is needed if I want
    //       to pass in the block of the matrix
    virtual void gradient(Eigen::Block<Eigen::MatrixXd,-1,-1>&& g) override 
    {
      // This depends on the forward pass
      Grad_.block(0, outSize_, outSize_, outSize_*inSize_) = makeRecursive(dependencies_[0]->GetValue(), 
                                                                              Eigen::Index(outSize_), 
                                                                              Eigen::Index(outSize_*inSize_));
      g = gradient_ * Grad_;
    }

    virtual void initialise_parameters(bool force = false)
    {
      if (!initialisedParameters_ || force) {
        weights_ = RandomInit<Gaussian>(outSize_, inSize_, InitDistribution_);
        biases_ = Eigen::VectorXd::NullaryExpr(outSize_, InitDistribution_);
        initialisedParameters_ = true;
      }
    }
    
    // Initialise flag missing
    void initialise_gradients(size_t row, bool force = false)
    { 
      if (!initialisedGradients_ || force) {
        Grad_ = Eigen::MatrixXd::Zero(outSize_, outSize_*(inSize_ + 1) );
        Grad_.block(0,0, outSize_, outSize_) = Eigen::MatrixXd::Identity(outSize_, outSize_);
        SetGradient(Eigen::MatrixXd::Zero(row, outSize_));
        initialisedGradients_ = true;
      }
    }

    virtual void initialise(size_t row, bool force = false)
    {
      initialise_parameters(force);
      initialise_gradients(row, force);
    }

    size_t getParNumber() const { return numberOfParameters_; }

  private:
    Eigen::MatrixXd weights_;
    Eigen::VectorXd biases_;
    Eigen::MatrixXd Grad_;
    bool initialisedGradients_ = false;
    bool initialisedParameters_ = false;
    size_t numberOfParameters_;
    InitDistribution InitDistribution_;
};
#endif
