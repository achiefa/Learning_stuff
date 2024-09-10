#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string
#include <bitset>

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
  template <typename ... Args> static auto MakeObservable (Type obs_type, Args&& ... args);
};

class BasicObservable : public Observable {
  public:
  BasicObservable(std::string s1, std::bitset<4> options) { std::cout << "Basic observable created: " << s1 << std::endl; }
  ~BasicObservable() { std::cout << "Basic observable destructed" << std::endl; }
  static constexpr std::uint8_t atInit    { 1 << 0 };
  static constexpr std::uint8_t contract  { 1 << 2 };
  static constexpr std::uint8_t fullData  { 1 << 3 };
  static constexpr std::uint8_t optimise  { 1 << 3 };
};

class CombinedObservable : public Observable {
  public:
  CombinedObservable(std::string s1, std::string s2, std::bitset<3> options ) { std::cout << "Combined observable created: " << s1 << " and " << s2 << std::endl; }
  ~CombinedObservable() { std::cout << "Combined observable destructed" << std::endl; }
  static constexpr std::uint8_t atInit    { 1 << 0};
  static constexpr std::uint8_t contract  { 1 << 2};
  static constexpr std::uint8_t fullData  { 1 << 3};
};

template <typename ... Args> auto Observable::MakeObservable (Type obs_type, Args&& ... args) {
  typedef std::unique_ptr<Observable> result_t;
      switch (obs_type) {
        case BASIC :
          return result_t{new BasicObservable(std::forward<Args> (args) ...)};
        case COMBINED : return result_t{new CombinedObservable(std::forward<Args> (args) ...)};
        default : abort();
      }
}

int main() {
  std::unique_ptr<Observable> basic = Observable::MakeObservable(Observable::BASIC);
  std::unique_ptr<Observable> combined = Observable::MakeObservable(Observable::COMBINED);
  return 0;
}