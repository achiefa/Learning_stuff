/**
 * @file abstract_object.hpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <utility>
#include <memory>

namespace core {
    
    /**
   * @brief Abstract implementation of objects that will be stored in
   * the type_map. This class does not own the lifetime of the object
   * it wraps. Such lifetime is controlled by the injector.
   * 
   */
  class abstract_instance_container {
    public:
      virtual ~abstract_instance_container() = default;

      /**
       * @brief The type_map that holds the instance container has information
       * about the type. This allows us to safely convert the raw pointer `void*`
       * to the original type.
       * 
       * @return void* 
       */
      void* get() { return raw_ptr_; }

      abstract_instance_container(abstract_instance_container &&other) {
        *this = std::move(other);
      }

      abstract_instance_container& operator=(abstract_instance_container &&other) {
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr;
        return *this;
      }

    protected:
      explicit abstract_instance_container(void *raw_ptr) :
        raw_ptr_(raw_ptr)
        { /* do nothing */}

    private:
      void* raw_ptr_;
  };


  template <class T, class Deleter>
  class instance_container : public abstract_instance_container {
    public:
      ~instance_container() override = default;

      explicit instance_container(std::unique_ptr<T, Deleter> &&p) :
        abstract_instance_container(p.get()),
        pointer_(std::move(p))
        { /* do nothing */}

      instance_container(instance_container &&other) {
        *this = std::move(other);
      }

      instance_container& operator=(instance_container &&other) {
        pointer_ = std::move(other); // Not really clear
        abstract_instance_container::operator=(std::move(other));
        return *this;
      }
    
    private:
      std::unique_ptr<T, Deleter> pointer_;
  };


  template <class T, class Deleter>
  std::unique_ptr<abstract_instance_container>
    wrap_into_instance_container(std::unique_ptr<T, Deleter> &&ptr) {
      return std::make_unique<instance_container<T, Deleter>> (std::move(ptr));
    }
    
} // namespace core
