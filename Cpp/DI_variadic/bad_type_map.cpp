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
  AbstractObject* get (const std::string &object_name) {
    auto it = _objects.find(object_name);
    assert(it != _objects.end());
    return it->second.get();
  }

  void add_object(std::unique_ptr<AbstractObject> ptr, std::string&& name) {
    _objects.insert({name, std::move(ptr)});
  }

  std::unordered_map<std::string, std::unique_ptr<AbstractObject>> _objects;
};

/**
 * @brief Unique identifier (Type register)
 * Implement a thread-safe counter, as usually done in **type identification**
 * systems (e.g. type registries), where each type gets a unique ID during
 * **runtime**, and the counter ensures that each new type registered gets
 * a different ID (even in a multi-threaded environment).
 *  
 * Each time `generate_type_id` is called 
 */
std::atomic_int TypeIdCounter(0);

template <typename T>
int generate_type_id() {
  // `static` local variables are initialized ony once per instantiation
  // of the function template. Once they are initialized, they retain their
  // value across subsequent calls. So they will always return the same 
  // if the function is called with the same type `T`.
  static int id = TypeIdCounter.fetch_add(1);
  return id;
}


int main(){

  Simulation simulation;
  std::unique_ptr<AbstractObject> obj1 = std::make_unique<Object1>();
  std::unique_ptr<AbstractObject> obj2 = std::make_unique<Object2>(2);
  simulation.add_object(std::move(obj1), "obj 1");
  simulation.add_object(std::move(obj2), "obj 2");
  simulation.add_object(std::move(obj1), "obj 2 but should be 1");


  /**
   * There are to two problems with the simple hash table implemented
   * in the `Simulation` class.
   * 
   * 1. It is not safe, meaning that it **not guaranteed** that 
   * `simulation.get("obj 1")` will return `Object1`. The method `get()`
   * return a pointer to the base class `AbstractObject*`, which could
   * point to any the derived class, and there's no type-checking mechanism
   * at runtime to ensure correctness. See the following example:
   */

  // Down-casting
  AbstractObject* obj = simulation.get("obj 2 but should be 1");
  //static_cast<Object2*>(obj)->method(); // That's really dangerous.

  /**
   * If `obj` is not actually an `Object2` but an `Object1` (for instance,
   * an error in filling the map), then this `static_cast` is invalid and
   * can lead to a segmentation fault or other runtime issues.
   * 
   * 2. Then we could use `dyanmic_cast` to safely downcast the base class
   * pointer to a derived class pointer. If the `dynamic_cast` fails, it
   * returns `nullptr`, allowing the user to handle the error safely
   */
  if (Object2* obj2 = dynamic_cast<Object2*>(obj))
    obj2->method();
  else
    std::cout << "Error: The object is not Object2." << std::endl;

  /**
   * This means that each time we want to work with the map, we have
   * to deal the the awkward cast
   */
  dynamic_cast<Object2*>(simulation.get("obj 2"))->method();

  /**
   * It would be great if it was possible to write
   * simulation.get<Object2>()->method();
   */
  std::cout << generate_type_id<double>() << std::endl;
  std::cout << generate_type_id<int>() << std::endl;
  std::cout << generate_type_id<double>() << std::endl;
}