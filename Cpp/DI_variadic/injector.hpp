/**
 * @file injector.hpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief Example of dependency injection in c++ using Variadic Templates.
 * Taken from https://gpfault.net/posts/dependency-injection-cpp.txt.html.
 * @version 0.1
 * @date 2024-10-06
 * 
 * 
 */

#include <utility>
#include <memory>

#include "./type_map.hpp"
#include "./abstract_object.hpp"

/**
 * @brief Alias template for a function pointer type that accepts a set of
 * parameters and returns an instance of `std::unique_ptr` for the type
 * `InstanceType`.
 * 
 * @tparam InstanceType 
 * @tparam Deleter 
 * @tparam Deps 
 */
template <class InstanceType, class Deleter, class ...Deps>
using instance_factory_function =
  std::unique_ptr<InstanceType, Deleter> (*)(Deps* ...);


class injector {
  public:
    /**
     * @brief Move constructor
     * 
     */
    injector(injector &&other) { *this = std::move(other); }

    /**
     * @brief Overloading of the operator `=`.
     * 
     * @param other 
     * @return injector& 
     */
    injector& operator=(injector &&other) { intance_map_ = std::move(other.instance_map_); }

    /**
     * @brief Function template that returns a pointer to the instance of
     * an object in the injector
     * 
     * @details When calling `injector.get_instance<T>()` the function will
     * return a pointer to the object of type `T` by resolving and injecting
     * all the dependencies that the object `T` needs to be initialised. It
     * also means that the object `T` and the dependencies must be registered
     * with the injector. Failing in that results in an exception.
     * 
     * @tparam T 
     * @tparam Dependent 
     * @return T* 
     */
    template <class T, class Dependent = std::nullptr_t>
    T* get_instance() const;

    /**
     * @brief The `inject` function invokes the object factory and bounds
     * each parameter of the `inject` function to the object registered
     * in the `injector`, returning the `std::unique_ptr` returned by the
     * object factory.
     * 
     * @tparam InstanceType 
     * @tparam Deleter 
     * @tparam Deps 
     * @param instance_factory 
     * @return std::unique_ptr<InstanceType, Deleter> 
     */
    template <class InstanceType, class Deleter, class ...Deps>
    std::unique_ptr<InstanceType, Deleter> inject(
      instance_factory_function<InstanceType, Deleter, Deps ...> instance_factory) const;

  private:
    injector() = default;
    using instance_map = 
     core::type_map<std::unique_ptr<core::abstract_instance_container>>;
    instance_map instance_map_;
};