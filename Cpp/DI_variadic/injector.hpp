/**
 * @file injector.hpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief Example of dependency injection in c++ using Variadic Templates.
 * Taken from https://gpfault.net/posts/dependency-injection-cpp.txt.html.
 * @version 0.1
 * @date 2024-10-06
 * 
 * The class `injector` does not provide any method to add service instances
 * to the injector. This is intentional, as services need to be added in the
 * correct order. If a service is added before its dependencies, it would
 * result in an error. The correct order of the dependencies is addressed 
 * by the class `di_config`.
 */

#include <utility>
#include <memory>

#include "type_map.hpp"
#include "abstract_object.hpp"

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
using instance_factory_function = std::unique_ptr<InstanceType, Deleter> (*)(Deps* ...);
using instance_map = core::type_map<std::unique_ptr<core::abstract_instance_container>>;

class di_config;

class injector {
    friend class di_config;
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
    injector& operator=(injector &&other) { 
      instance_map_ = std::move(other.instance_map_); 
      return *this;
    }

    /**
     * @brief Function template that returns a pointer to the instance of
     * an object in the injector
     * 
     * @details When calling `injector.get_instance<T>()` the function will
     * return a pointer to the instance of the object `T` in the injector with
     * the dependencies already resolved. It also means that the object `T` and 
     * the dependencies must be registered with the injector. Failing in that 
     * results in an exception.
     * 
     * `Dependent` ????
     * 
     * @tparam T 
     * @tparam Dependent 
     * @return T* 
     */
    template <class T, class Dependent = std::nullptr_t>
    T* get_instance() const {
      auto it = instance_map_.find<T>();
      if (it == instance_map_.end()) {
        throw std::runtime_error(
          std::string(typeid(T).name()) + ": unsatisfied dependency of " + std::string(typeid(Dependent).name())
        );
      }
      return static_cast<T*>(it->second->get());
    }

    /**
     * @brief The `inject` function invokes the factory function pointed by
     * `instance_factory`. Each parameter of the factory function is bound to
     * the appropriate service (object) instance. It will return the 
     * `std::unique_ptr` specified by the factory.
     * 
     * The ellipsis at the end of the call are needed in the context of
     * variadic template. They cause the expression to be repeated for each
     * element in the parameter pack `Deps`. For example, if the instance
     * factory  passed to inject looks like
     * 
     * ``` code
     * 
     *    std::unique_ptr<Service1> Service1Factory(Service2* s2, Service3* s3) {
     *      return std::make_unique<Service1>(s2, s3);
     *    }
     * ```
     * Then the call inside `inject` is equivalent to the following
     * 
     * ``` code
     *    instance_factory(
     *      get_instance<typename std::remove_const<Service2>::type,
     *                   typename std::remove_const<Service1>::type>()
     *      get_instance<typename std::remove_const<Service3>::type,
     *                   typename std::remove_const<Service1>::type>());
     * ```
     * 
     * @tparam InstanceType 
     * @tparam Deleter 
     * @tparam Deps 
     * @param instance_factory 
     * @return std::unique_ptr<InstanceType, Deleter> 
     */
    template <class InstanceType, class Deleter, class ...Deps>
    std::unique_ptr<InstanceType, Deleter> inject(
      instance_factory_function<InstanceType, Deleter, Deps ...> instance_factory) const {
        return instance_factory(
          get_instance<typename std::remove_const<Deps>::type,
                       typename std::remove_const<InstanceType>::type>()...);
      }

  private:
    injector() = default;
    instance_map instance_map_;
};