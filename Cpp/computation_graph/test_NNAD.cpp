#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <NNAD/FeedForwardNN.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "nodes.hpp"


Eigen::VectorXd to_eigen_vector(const std::vector<double>& vec)
{
  Eigen::VectorXd eigen_vec;
  double x[vec.size()];
  std::copy(vec.begin(), vec.end(), x);
  Eigen::Map<Eigen::RowVectorXd> test(x, vec.size());
  eigen_vec = test.eval();
  return eigen_vec;
}


Eigen::VectorXd to_eigen_vector(const nnad::Matrix<double>& vec)
{
  Eigen::VectorXd evec = Eigen::Map<Eigen::VectorXd>(vec.GetVector().data(), vec.GetLines());
  return evec;
}

Eigen::MatrixXd to_eigen_matrix(const nnad::Matrix<double>& mat)
{
  Eigen::MatrixXd emat = Eigen::Map<Eigen::Matrix<double, 
                                                 Eigen::Dynamic, 
                                                 Eigen::Dynamic, 
                                                 Eigen::RowMajor>>(mat.GetVector().data(), 
                                                                   mat.GetLines(), 
                                                                   mat.GetColumns());
  return emat;
}

int main()
{
  auto console = spdlog::stdout_color_mt("App");

  // Input vector
  std::vector<double> x{0.1, 2.3, 4.5};
  Eigen::VectorXd vec_x = to_eigen_vector(x);

  // Define architecture
  const std::vector<int> arch{3, 2, 2, 1};

  // Initialise NN
  const nnad::FeedForwardNN<double> nn{arch, 
                                       0,
                                       false,
                                       nnad::Tanh<double>,
                                       nnad::dTanh<double>,
                                       nnad::OutputFunction::ACTIVATION
                                      };

  Input input(x.size());
  input.setValues(to_eigen_vector(x));
  std::unique_ptr<Node> layer1 (new Linear(arch[1], {&input}, "layer 1"));
  std::unique_ptr<Node> layer1_act (new Activation<Tanh>({layer1.get()}, "activation layer 1"));
  std::unique_ptr<Node> layer2 (new Linear(arch[2], {layer1_act.get()}, "layer 2"));
  std::unique_ptr<Node> layer2_act (new Activation<Tanh>({layer2.get()}, "activation layer 2"));
  std::unique_ptr<Node> layer3 (new Linear(arch[3], {layer2_act.get()}, "layer 3"));
  std::unique_ptr<Node> output (new Activation<Tanh>({layer3.get()}, "output"));
  input.initialise(output->GetOutSize(), false);
  static_cast<Linear*> (layer1.get())->initialise_gradients( output->GetOutSize());
  static_cast<Linear*> (layer2.get())->initialise_gradients( output->GetOutSize());
  static_cast<Linear*> (layer3.get())->initialise_gradients( output->GetOutSize());
  static_cast<Activation<Tanh>*> (layer1_act.get())->initialise_gradients( output->GetOutSize());
  static_cast<Activation<Tanh>*> (layer2_act.get())->initialise_gradients( output->GetOutSize());
  static_cast<Activation<Tanh>*> (output.get())->initialise_gradients( output->GetOutSize());
  
  static_cast<Linear*> (layer1.get())->setBiases(to_eigen_vector(nn.GetBiases(1)));
  static_cast<Linear*> (layer1.get())->setWeights(to_eigen_matrix(nn.GetLinks(1)));
  static_cast<Linear*> (layer2.get())->setBiases(to_eigen_vector(nn.GetBiases(2)));
  static_cast<Linear*> (layer2.get())->setWeights(to_eigen_matrix(nn.GetLinks(2)));
  static_cast<Linear*> (layer3.get())->setBiases(to_eigen_vector(nn.GetBiases(3)));
  static_cast<Linear*> (layer3.get())->setWeights(to_eigen_matrix(nn.GetLinks(3)));

  output->forward();
  spdlog::info("Printing the output:");
  std::cout << output->GetValue() << std::endl;

  spdlog::info("Printing NNAD result");
  auto eval = nn.Evaluate(x);
  for (auto& e : eval)
    std::cout << e;
  std::cout << std::endl;

  spdlog::info("Check gradient...");

  output->SetGradient(Eigen::MatrixXd::Identity(output->GetOutSize(), output->GetOutSize()));
  output->backward();
  layer3->backward();
  layer2_act->backward();
  layer2->backward();
  layer1_act->backward();
  layer1->backward();

  size_t Npar = 0;
  int Npar1 = static_cast<Linear*> (layer1.get())->getParNumber();
  int Npar2 = static_cast<Linear*> (layer2.get())->getParNumber();
  int Npar3 = static_cast<Linear*> (layer3.get())->getParNumber();
  int Nb1 = static_cast<Linear*> (layer1.get())->number_of_biases;
  int Nb2 = static_cast<Linear*> (layer2.get())->number_of_biases;
  int Nb3 = static_cast<Linear*> (layer3.get())->number_of_biases;
  int Nw1 = static_cast<Linear*> (layer1.get())->number_of_weights;
  int Nw2 = static_cast<Linear*> (layer2.get())->number_of_weights;
  int Nw3 = static_cast<Linear*> (layer3.get())->number_of_weights;
  Npar += static_cast<Linear*> (layer1.get())->getParNumber();
  Npar += static_cast<Linear*> (layer2.get())->getParNumber();
  Npar += static_cast<Linear*> (layer3.get())->getParNumber();
  Eigen::MatrixXd J = Eigen::MatrixXd::Zero(layer3->GetOutSize(), Npar);
  layer3->gradient(J.block(0, 
                           Npar - Npar3,
                           layer3->GetOutSize(), 
                           Npar3));
  layer2->gradient(J.block(0, 
                           Npar - Npar3 - Npar2,
                           layer3->GetOutSize(), 
                           Npar2));
  layer1->gradient(J.block(0, 
                           Npar - Npar3 - Npar2 - Npar1,
                           layer3->GetOutSize(), 
                           Npar1));


  auto derivative = nn.Derive(x);
  std::cout << J.transpose() << std::endl;
  //std::cout << "___________" << std::endl;
  //int counter = 0;
  //for (auto& j : derivative) {
  //  if (counter > 0)
  //    std::cout << j << std::endl;
  //  counter++;
  //}


  return 0;
}