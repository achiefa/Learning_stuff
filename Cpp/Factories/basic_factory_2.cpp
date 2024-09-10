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
 * @brief Base class
 * 
 * In this case, both type identifiers and factory method have been
 * moved into the base class. This improves encapsulation keep things
 * together
 * 
 */
class Observable {
  public:
  enum Type {BASIC, COMBINED};
  virtual ~Observable() {}
  static auto MakeObservable (Type obs_type);
};

class BasicObservable : public Observable {
  public:
  BasicObservable() { std::cout << "Basic observable created" << std::endl; }
  ~BasicObservable() { std::cout << "Basic observable destructed" << std::endl; }
};

class CombinedObservable : public Observable {
  public:
  CombinedObservable() { std::cout << "Combined observable created" << std::endl; }
  ~CombinedObservable() { std::cout << "Combined observable destructed" << std::endl; }
};

auto Observable::MakeObservable (Type obs_type) {
  typedef std::unique_ptr<Observable> result_t;
      //using result_t = std::unique_ptr<Observable>;
      switch (obs_type) {
        case BASIC : return result_t{new BasicObservable};
        case COMBINED : return result_t{new CombinedObservable};
        default : abort();
      }
}

int main() {
  std::unique_ptr<Observable> basic = Observable::MakeObservable(Observable::BASIC);
  std::unique_ptr<Observable> combined = Observable::MakeObservable(Observable::COMBINED);
  return 0;
}