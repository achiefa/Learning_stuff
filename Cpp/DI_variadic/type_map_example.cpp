/**
 * @file type_map.hpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief Implementation of the the type map. Type maps are special kind
 * of has table, where keys are types. Type maps are needed to store objects
 * into the injector such that we are able to query objects by type.
 * @version 0.1
 * @date 2024-10-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <unordered_map>
#include <string>
#include <memory>
#include <atomic>
#include <cassert>
#include <iostream>
#include <atomic>
#include "type_map.hpp"


struct AbstractObject
{
  virtual ~AbstractObject() = default;
};

struct Object1 : AbstractObject
{
  void method()  { std::cout<< "Object 1" << std::endl; }
};


struct Object2 : AbstractObject
{
  Object2 (int&& a): _a(a) {}
  void method()  { std::cout<< "Object 2 : " << _a << std::endl; }
  int _a;
};

struct Simulation
{
  template <typename Object>
  Object* get () {
    auto it = _objects.find<Object>();
    assert(it != _objects.end());
    return static_cast<Object*>(it->second.get());
  }

  template <typename Object>
  void add_object(std::unique_ptr<Object> &&obj) {
    _objects.put<Object>(std::forward<std::unique_ptr<Object>>(obj));
  }

  core::type_map<std::unique_ptr<AbstractObject>> _objects;
};

int main(){

  Simulation simulation;
  simulation.add_object(std::make_unique<Object1>());
  simulation.add_object(std::make_unique<Object2>(2));
  simulation.add_object(std::make_unique<Object2>(3));
  simulation.add_object(std::make_unique<Object2>(4));

  simulation.get<Object1>()->method();
  simulation.get<Object2>()->method();
  
}