#include "./nodes.hpp"
#include <iostream>
#include "spdlog/sinks/stdout_color_sinks.h"

int main()
{
  auto console = spdlog::stdout_color_mt("App");
  Eigen::VectorXd x {{3.0, 1.0, 24.}};

  //Node::NodePtr input = new Input(3);
  //input->setValues(std::move(x));
  Input input(3);
  input.setValues(std::move(x));
  

  std::mt19937 generator(10);
  auto parameters = std::make_tuple(0.0,1.0);
  std::unique_ptr<Node> layer1 (new Linear(10, {&input}, "layer 1"));
  std::unique_ptr<Node> layer1_act (new Activation<Tanh>({layer1.get()}, "activation layer 1"));
  std::unique_ptr<Node> layer2 (new Linear(10, {layer1_act.get()}, "layer 2"));
  std::unique_ptr<Node> layer2_act (new Activation<Tanh>({layer2.get()}, "activation layer 2"));
  std::unique_ptr<Node> layer3 (new Linear(10, {layer2_act.get()}, "layer 3"));

  // TODO: Get rid of this static_cast. This can be done by using CRTP fo instance
  // I hate this static cast. The problem is that the base class does not
  // know anything about the derived class
  static_cast<Linear*> (layer1.get())->initialise( layer3->GetOutSize(), &generator, parameters, false);
  static_cast<Linear*> (layer2.get())->initialise( layer3->GetOutSize(), &generator, parameters, false);
  static_cast<Linear*> (layer3.get())->initialise( layer3->GetOutSize(), &generator, parameters, false);
  static_cast<Activation<Tanh>*> (layer1_act.get())->initialise_gradients( layer3->GetOutSize());
  static_cast<Activation<Tanh>*> (layer2_act.get())->initialise_gradients( layer3->GetOutSize());
  spdlog::info("Sono qui");
  input.initialise(layer3->GetOutSize(), false);

  spdlog::info("Performing forward pass...");
  layer3->forward();
  spdlog::info("Printing the output:");
  std::cout << layer3->GetValue() << std::endl;


  spdlog::info("Starting backward process");
  // Initialise gradient matrix
  layer3->SetGradient(Eigen::MatrixXd::Identity(layer3->GetOutSize(), layer3->GetOutSize()));
  layer3->backward();
  layer2_act->backward();
  layer2->backward();
  layer1_act->backward();
  layer1->backward();

  spdlog::info("Now I initialise the gradient");
  // Collecting the total number of parameters
  // TODO This part must be revisited. First, the network also
  // has activation nodes. Second, I want to implement the possibility
  // to `freeze` the layer, and thus the MLP must be aware of that.
  size_t Npar = 0;
  Npar += static_cast<Linear*> (layer1.get())->getParNumber();
  Npar += static_cast<Linear*> (layer2.get())->getParNumber();
  Npar += static_cast<Linear*> (layer3.get())->getParNumber();
  Eigen::MatrixXd J = Eigen::MatrixXd::Zero(layer3->GetOutSize(), Npar);
  layer3->gradient(J.block(0, 
                           Npar - static_cast<Linear*> (layer3.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear*> (layer3.get())->getParNumber()));
  layer2->gradient(J.block(0, 
                           Npar - static_cast<Linear*> (layer3.get())->getParNumber() - static_cast<Linear*> (layer2.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear*> (layer2.get())->getParNumber()));
  layer1->gradient(J.block(0, 
                           Npar - static_cast<Linear*> (layer3.get())->getParNumber() - static_cast<Linear*> (layer2.get())->getParNumber() - static_cast<Linear*> (layer1.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear*> (layer1.get())->getParNumber()));
  std::cout << J << std::endl;


  //Activation<Tanh> act({&layer5}, "test");
  //Activation<Identity> test({&layer5}, "test");
  //CustomActivation MyAct ([](const double& x) { return x/2; }, [](const double& x) { return 1./2.; });
  //Activation test({&layer5}, "test", MyAct);
  //spdlog::info("Applying activation");
  //std::cout << test.GetValue() << std::endl;
  //delete input;
  return 0;
}