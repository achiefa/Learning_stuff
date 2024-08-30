#include "./node.hpp"
#include "distribution_policies.hpp"
#include "utils.hpp"
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#ifndef INCLUDE_LINEAR_NODE
#define INCLUDE_LINEAR_NODE

/**
 * @todo
 * - copy constructor
 * - move constructor
 * - assignment operator
 * - ...
 * 
 */
class Linear : public Node
{ 
  public:
    Linear(size_t&& size, 
           VecDep&& dependencies, 
           std::string&& Id)
    : Node(std::forward<VecDep>(dependencies), std::forward<std::string>(Id))
    {
      outSize_ = size;
      if (!dependencies_.empty()) {
        inSize_ = dependencies_[0]->GetOutSize();
      }
      numberOfParameters_ = outSize_ * (inSize_ + 1);

      number_of_biases = outSize_;
      number_of_weights = outSize_ * inSize_;
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
      // This is a rather generic implementation of the backward pass, and I think that could
      // be implemented in the base class. The part that is specific to the particular type of
      // node (the linear node in this case) is `gradient * weight`
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

    void setWeights(const Eigen::MatrixXd& weights) 
    {
      weights_ = weights;
    }

    void setBiases(const Eigen::VectorXd& biases) 
    {
      biases_ = biases;
    }

    Eigen::MatrixXd GetWeights () const { return weights_; }
    Eigen::VectorXd GetBiases() const { return biases_; }

    template <template<typename> typename distribution = Gaussian, typename RNG, typename ...Params>
    void initialise_parameters(RNG* rng, 
                               const std::tuple<Params...>& t = std::tuple<double,double>(0.,1.), 
                               bool force = false)
    {
      if (!initialisedParameters_ || force) {
        weights_ = RandomInit<distribution>(outSize_, inSize_, rng, t);
        biases_ = RandomInit<distribution>(outSize_, rng, t);
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

    template <template<typename> typename distribution = Gaussian, typename RNG, typename ...Params>
    void initialise(size_t row, 
                    RNG* rng, 
                    const std::tuple<Params...>& t = std::tuple<double,double>(0.,1.),
                    bool force = false)
    {
      initialise_parameters(rng, t, force);
      initialise_gradients(row, force);
    }

    size_t getParNumber() const { return numberOfParameters_; }
    size_t number_of_weights;
    size_t number_of_biases;

  private:
    Eigen::MatrixXd weights_;
    Eigen::VectorXd biases_;
    Eigen::MatrixXd Grad_; // This is really an auxiliary matrix
    bool initialisedGradients_ = false;
    bool initialisedParameters_ = false;
    size_t numberOfParameters_;
};
#endif
