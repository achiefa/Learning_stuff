#include <vector>
#include <string>
#include <functional>
#include <eigen3/Eigen/Eigen>
#include "spdlog/spdlog.h"
#include <iostream>
#include <any>

#ifndef INCLUDE_NODE
#define INCLUDE_NODE

/**
 * @brief Base Node class
 * 
 * It would be better to apply CRTP to the base class
 */
class Node
{ 
  public:
    using NodePtr = Node*;
    using VecDep = std::vector<NodePtr>;

    Node(VecDep&& dependencies, std::string&& Id)
    : dependencies_(std::move(dependencies)), 
      Id_(std::move(Id)),
      dirtyFlag_(false)
    {/* do nothing */}

    virtual ~Node() {};

    virtual void forward() {};

    virtual void backward() {};

    virtual void gradient(Eigen::Block<Eigen::MatrixXd,-1,-1>&& g) {};

    virtual void dependency_rule() {}

    virtual void setValues(Eigen::VectorXd&& x)
    { 
      value_ = std::move(x);
      dirtyFlag_ = false;
    }

    // Getters
    std::string GetId() const { return Id_; }
    size_t GetInSize() const { return inSize_; }
    size_t GetOutSize() const { return outSize_; }
    Eigen::MatrixXd GetGradient() const { return gradient_; }
    void SetGradient(Eigen::MatrixXd&& gradient) { this->gradient_ = gradient; }
    VecDep GetDependencies() const { return dependencies_; }
    const Eigen::VectorXd GetValue() 
    {  
      if (dirtyFlag_)
        return value_;
      else {
        forward();
        dirtyFlag_ = true;
        return value_;
      } 
    }

  protected:
    Eigen::VectorXd value_;
    Eigen::MatrixXd gradient_;
    size_t outSize_;
    size_t inSize_;
    VecDep dependencies_;
    std::string Id_;
    bool dirtyFlag_;

    enum NodeType {
      ExternalNode,
      InternalNode
    };
};
#endif