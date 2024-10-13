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

#ifndef TYPE_MAP_HEAEDER_H
#define TYPE_MAP_HEAEDER_H

#include <unordered_map>
#include <atomic>

namespace core {
  /**
   * @brief Implementation of the `type_map` container. Effectively,
   * this is a wrapper to `std::unordered_map`
   * 
   * @tparam ValueType 
   */
  template <class ValueType>
  class type_map {
    using InternalMap = std::unordered_map<int, ValueType>;
  public:
    using iterator = typename InternalMap::iterator;
    using const_iterator = typename InternalMap::const_iterator;
    using value_type = typename InternalMap::value_type;

    iterator begin() { return _map.begin(); }
    iterator end() { return _map.end(); }
    const_iterator begin() const { return _map.begin(); }
    const_iterator end() const { return _map.end(); }
    const_iterator cbegin() const { return _map.cbegin(); }
    const_iterator cend() const { return _map.cend(); }

    template <class Key>
    iterator find() { return _map.find(type_id<Key>()); }

    template <class Key>
    const_iterator find() const { return _map.find(type_id<Key>()); }

    template <class Key>
    void put(ValueType &&value) {
      if (contains<Key>()) {
        std::string name (typeid(Key).name());
        throw std::runtime_error("Object " + name + " already present");
      }
      else
        _map[type_id<Key>()] = std::forward<ValueType>(value);
    }

    template <class Key>
    bool contains() { return _map.contains(type_id<Key>()); }

    template <class Key>
    static int type_id() {
      static int id = ++last_type_id;
      return id;
    }

  private:

    static std::atomic<int> last_type_id;
    InternalMap _map;
  };

  template <class T>
  std::atomic<int> type_map<T>::last_type_id(0);
} // namespace core

    
#endif