/**
 * @file app.cpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "di_config.hpp"
#include <iostream>

struct Service1 {};
struct Service2 {};
struct Service3 {
  Service3(Service1*, Service2*) { 
    std::cout << "Magic" << std::endl; 
  }
};

std::unique_ptr<Service1> Service1Factory() {
  return std::make_unique<Service1>(); // Just pass the arguments to Service1's ctor.
}

std::unique_ptr<Service2> Service2Factory() {
  return std::make_unique<Service2>(); // Just pass the arguments to Service1's ctor.
}

std::unique_ptr<Service3> Service3Factory(Service1* s1, Service2* s2) {
  return std::make_unique<Service3>(s1, s2); // Just pass the arguments to Service1's ctor.
}

template <class InstanceType, class ...Deps>
std::unique_ptr<InstanceType> generic_instance_factory(Deps*... deps) {
  return std::make_unique<InstanceType>(deps...);
}

int main() {

  di_config cfg;
  cfg.add(Service1Factory);
  cfg.add(Service2Factory);
  cfg.add(Service3Factory);
  injector inj = cfg.build_injector();

  di_config cfg2;
  cfg2.add(generic_instance_factory<Service1>);
  cfg2.add(generic_instance_factory<Service2>);
  cfg2.add(generic_instance_factory<Service3, Service2, Service1>);
  injector inj2 = cfg2.build_injector();


  return 0;
}