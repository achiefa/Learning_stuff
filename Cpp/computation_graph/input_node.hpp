#include "./node.hpp"

#ifndef INCLUDE_INPUT_NODE
#define INCLUDE_INPUT_NODE


/**
 * @brief Specification of the linear class for the input case.
 * 
 * @todo This class should be revisited...
 */
class Input : public Node
{ 
  public:
    Input(size_t&& size, VecDep&& dependencies = {}, std::string&& Id = std::string("input"))
    : Node(std::forward<VecDep>(dependencies), std::forward<std::string>(Id))
    { 
      outSize_ = std::move(size);
    }

    ~Input() {}
    
    void forward() override { 
      dirtyFlag_ = true; }

    void dependency_rule() override {}


    // This must change, I don't like it
    void initialise(size_t row, bool force = false)
    {
      SetGradient(Eigen::MatrixXd::Zero(row, outSize_));
    }

  private:

    //void backward() override {}
    //void gradient() override {}
};
#endif