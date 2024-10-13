/**
 * @file di_config.hpp
 * @author Amedeo Chiefa (amedeochiefa@gmail.com)
 * @brief Implementation of the client injector `di_config`.
 * @version 0.1
 * @date 2024-10-13
 * 
 * The client code cannot create an `injector` object directly, nor adding
 * services to it. It can only set up a `di_config` object which will create
 * the injector. 
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <string>
#include <unordered_set>
#include <stack>

#include "injector.hpp"
#include "type_map.hpp"

/**
 * @brief Generate and resolve graph of dependencies
 * 
 * The nodes in this graph are the instance factories. There is an edge
 * between nodes A and B iff the instance factory B requires the service
 * produced by the instance factory A. The order in which services are added
 * to the injector is determined by doing a topological sort of the dependency
 * graph.
 * 
 */
class di_config {
public:
  /**
   * @brief Register a new instance **factory**.
   * 
   * Instance factories can be registered in arbitrary order as no instances are
   * actually created at the time of registration.
   * 
   * @tparam InstanceType 
   * @tparam Deleter 
   * @tparam Deps 
   * @param instance_factory 
   */
  template <class InstanceType, class Deleter, class ...Deps>
  void add(instance_factory_function<InstanceType, Deleter, Deps...> instance_factory);

  /**
   * @brief Create a new injector with services created by the registered
   * instance factories.
   * 
   * This method is where the dependencies are actually resolved and passed to
   * factories. This method also ensures that instances are created in the
   * correct order.
   * 
   * @return injector 
   */
  injector build_injector();

private:
  using initializer_fn = std::function<void(injector&)>;

  void toposort_visit_node(int node_id,
                           std::unordered_set<int> *unmarked_nodes,
                           std::stack <di_config::initializer_fn*> *output);

  struct node_info {
    // Mark is needed for the topological sort algorithm
    enum class mark {
      UNMARKED, TEMP, PERM
    };
    mark mark_;

    // Name of the service class    
    std::string debug_type_name_;

    // A function that invokes the instance factory
    // and adds the created service to the given injector
    initializer_fn initializer_;
    bool has_initializer_ = false;

    // List of nodes adjacent to this one (dependencies and dependents)
    std::unordered_set<int> dependents_;
  };

  std::unordered_map<int, node_info> graph_;
};


template <class ...Args>
inline void passthru(Args... args) {}
/**
 * 1. Locate the graph node corresponding  to the `InstanceType` (the type
 *    of the service we want to use). If that node doesn't exist yet, it will
 *    be created.
 * 2. Assign an initializer function to the node. The initializer function is
 *    any callable that creates the service (passing its dependencies through
 *    the corresponding factory `instance_factory`) and then places it into the
 *    given injector. Note that, in order for `inj.inject()` to pass all the
 *    needed dependencies to the instance factory, these dependencies must have
 *    been added previously to the graph.
 * 3. For each dependence in the variadic template parameter `Deps`, an edge is
      added between the new node and the node in the graph corresponding to the 
      dependency. The function `passthru` is needed in order to expand the 
      parameter pack, thus using each parameter in it.
 * 
 */
template <class InstanceType, class Deleter, class ...Deps>
void di_config::add(
  instance_factory_function<InstanceType, Deleter, Deps...> instance_factory) {

    // Generate (or retrieve) the unique id using `type_id` method in `type_map`
    int instance_type_id = core::type_map<node_info>::type_id<typename std::remove_const<InstanceType>::type>();

    // If the key `instance_type_id` exists in `graph_`, it retrieves the
    // corresponding `node_info` object. If the key does not exist,
    // ``std::unordered_map` will automatically create a new entry
    // with the key `instance_type_id` and default-construct a `node_info`
    // object for that key.
    node_info &new_node_info = graph_[instance_type_id];

    new_node_info.initializer_ = [instance_factory](injector &inj) {
      auto instance = core::wrap_into_instance_container(inj.inject(instance_factory));
      inj.instance_map_.put<InstanceType>(std::move(instance));
    };
    new_node_info.has_initializer_ = true;
    new_node_info.debug_type_name_ = typeid(typename std::remove_const<InstanceType>::type).name();
    /**
     * @TODO Maybe this part can be improved using fold expression. Example
     * ```code
     *    
     *    template <class ...Args>
     *    inline void make_edge(Args... args) {
     *      ((
     *        graph_[core::type_map<node_info>::type_id<typename std::remove_const<Deps>::type>()]
              .dependents_
              .insert(instance_type_id)); 
              ), ...);
     *    }
     * ```
     */
    passthru(
      graph_[core::type_map<node_info>::type_id<typename std::remove_const<Deps>::type>()]
      .dependents_
      .insert(instance_type_id)...);
  }

  /**
   * @brief Build the injector
   * 
   * 1. Determine the correct order for calling the initializers by using the
   *    topological sort on the graph.
   * 2. Call the initializers in that order.
   * 
   * The stack `initializers` is initially empty. It is filled up, according
   * to the topological order, by `toposort_visit_node`. Then, the stack is
   * exhausted by calling each element.
   * @return injector 
   */
  injector di_config::build_injector() {
    injector inj;
    std::stack<initializer_fn*> initializers; // ??

    std::unordered_set<int> unmarked_nodes;
    for ( auto &node : graph_ ) {
      node.second.mark_ = node_info::mark::UNMARKED;
      unmarked_nodes.insert(node.first);
    }
    while (!unmarked_nodes.empty()) {
      int node_id = *unmarked_nodes.begin();
      toposort_visit_node(node_id, &unmarked_nodes, &initializers);
    }
    while (!initializers.empty()) {
      (*initializers.top())(inj);
      initializers.pop();
    }
    return std::move(inj);
  }

  /**
   * @brief Topological sort of the graph
   * 
   * @param node_id 
   * @param unmarked_nodes 
   * @param output 
   */
  void di_config::toposort_visit_node(
  int node_id,
  std::unordered_set<int> *unmarked_nodes,
  std::stack <di_config::initializer_fn*> *output) {
  node_info &info = graph_[node_id];
  if (info.mark_ == node_info::mark::TEMP) {
    throw std::runtime_error(info.debug_type_name_ + " appears to be part of a cycle");
  }
  else if (info.mark_ == node_info::mark::UNMARKED) {
    unmarked_nodes->erase(node_id);
    info.mark_ = node_info::mark::TEMP;
    for (int dependent : info.dependents_) {
      toposort_visit_node(dependent, unmarked_nodes, output);
    }
    info.mark_ = node_info::mark::PERM;
    if (info.has_initializer_) {
      // if has_initializer_ is false, it means someone depends on this
      // node, but an instance factory for this node has not been provided.
      // This will result in an injection error later.
      output->push(&info.initializer_);
    }
  }
}  