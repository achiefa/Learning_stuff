#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string

/**
 * @file basic_factory.cpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief Basic factory pattern
 * @version 0.1
 * @date 2024-05-27
 * 
 * The problem is how to decide at runtime to create an object
 * of a particular type. The Factory pattern is a creational 
 * pattern and, in its simplest from, the factory method
 * constructs an object of a type that's specified at runtime
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * @brief Runtime type identifier
 * 
 * In order to to identify at runtime which object to create, we
 * need a runtime identifier for each type that the factory can
 * create. In the simplest case, this list of types is know at
 * compile time.
 */
enum Observables {BASIC, COMBINED};
static std::unordered_map<std::string, Observables> const table = { {"BASIC", Observables::BASIC}, {"COMBINED", Observables::COMBINED } };

class Observable {
  public:
  virtual ~Observable() {}
};


class BasicObservable : public Observable {};
class CombinedObservable : public Observable {};

/**
 * @brief Factory function
 * 
 * Somewhere in the program, every object has to be explicitly constructed with its true type.
 * The Factory pattern does not remove this requirement, it merely hides the place where
 * the construction happens; in this case, in the switch. And at compile time, each case
 * is compiled and therefore each true constructor is seen by the compiler.
 * @param obs_type 
 * @return Observable* 
 */
Observable* MakeObservable (Observables obs_type) {
  switch (obs_type) {
    case BASIC: return new BasicObservable;
    case COMBINED: return new CombinedObservable;
  }
}


int main(int argc, char* argv[]) {
  Observables o_type;
  auto it = table.find(argv[1]);
  if (it != table.end())
    o_type = it->second;
  else
    std::cerr << "nope \n";

  //Creating the object with the factory method
  Observable* obs = MakeObservable(o_type);
  delete obs;
  return 0;
}