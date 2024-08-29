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
  

  //std::string  test = "layer_1";
  std::default_random_engine generator;
  double mean = 1.0;
  double var = 1.0;
  std::unique_ptr<Node> layer1 (new Linear(10, {&input}, "layer 1", Gaussian(mean,var, &generator)));
  std::unique_ptr<Node> layer2 (new Linear(10, {layer1.get()}, "layer 2",Gaussian(mean,var, &generator)));
  std::unique_ptr<Node> layer3 (new Linear(10, {layer2.get()}, "layer 3",Gaussian(mean,var, &generator)));

  // TODO: Get rid of this static_cast. This can be done by using CRTP fo instance
  // I hate this static cast. The problem is that the base class does not
  // know anything about the derived class
  static_cast<Linear<Gaussian>*> (layer1.get())->initialise( layer3->GetOutSize(), false);
  static_cast<Linear<Gaussian>*> (layer2.get())->initialise( layer3->GetOutSize(), false);
  static_cast<Linear<Gaussian>*> (layer3.get())->initialise( layer3->GetOutSize(), false);
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
  layer2->backward();
  layer1->backward();

  spdlog::info("Now I initialise the gradient");
  // Collecting the total number of parameters
  size_t Npar = 0;
  Npar += static_cast<Linear<Gaussian>*> (layer1.get())->getParNumber();
  Npar += static_cast<Linear<Gaussian>*> (layer2.get())->getParNumber();
  Npar += static_cast<Linear<Gaussian>*> (layer3.get())->getParNumber();
  Eigen::MatrixXd J = Eigen::MatrixXd::Zero(layer3->GetOutSize(), Npar);
  layer3->gradient(J.block(0, 
                           Npar - static_cast<Linear<Gaussian>*> (layer3.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear<Gaussian>*> (layer3.get())->getParNumber()));
  layer2->gradient(J.block(0, 
                           Npar - static_cast<Linear<Gaussian>*> (layer3.get())->getParNumber() - static_cast<Linear<Gaussian>*> (layer2.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear<Gaussian>*> (layer2.get())->getParNumber()));
  layer1->gradient(J.block(0, 
                           Npar - static_cast<Linear<Gaussian>*> (layer3.get())->getParNumber() - static_cast<Linear<Gaussian>*> (layer2.get())->getParNumber() - static_cast<Linear<Gaussian>*> (layer1.get())->getParNumber(),
                           layer3->GetOutSize(), 
                           static_cast<Linear<Gaussian>*> (layer1.get())->getParNumber()));
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