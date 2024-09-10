#include <iostream>
#include <functional>
#include <ranges>

// Trying to implement a pipeline of the type
// source |--- process_1 --- process_2 --- ... process_n ---| consumer
// 'consumer' can be any thing -- a function that prints the outcome
// of this chain, or anything that needs such outcome.

// The consumer must be satisfied. In other wants, we need to feed
// the consumer with an input, which could be the source itself. For
// instance, 'source' can be an array of unordered data, or word that
// we print through the consumer. Of course, since no internal transformation
// is applied, the consumer will print what it gets, namely unordered numbers
// or the string given as input
//
//   source |---| consumer.         (1)

// Say, however, that we want to organise the number in ascent order, or that
// we want to capitalise each letter of the word we have passed. This means that
// we need transform the source before it SATISFIES the consumer. Then we would
// have
//   source |--- transformed ---| consumer.         (1)
// Note that in this case `transform` becomes the new consumer wrt source,
// and source must satisfy `transform` and not `consumer`. These argument
// can be generalised to many sequential transformations, such that
// `transformation_n` becomes the consumer for `transformation_(n-1)`,
// which is the new source term. Note that `consumer` can be any
// other transformation, or the end-line function that uses this information.

// We have the same pattern (1) repeated many times. We can observe the
// following things:
// - `source` is the first element of the chain, but also any other
//   intermediate step. So for instance, in (1) `transform` is the
//   source. The `process` takes the `source` as input and gives 
//   the output as a new node.
// - The same pattern repeats many times...
// - ...and thus we have a finite number of objects to implement: `node`, 
//   `source`, and `process`
// Now `source` is just a different way for viewing a `node`.

template <class ...Ts>
struct Node : std::function<void(Ts...)>
{ 
  using std::function<void(Ts...)>::function;
};

template<class In, class Out>
struct BuildingTree : std::function<void(In,Out)>
{
  using std::function<void(In,Out)>::function;
};

template <class Ts>
std::function<void(Ts)> print_on_screen = [](Ts ts) {std::cout << ts << std::endl;};

int main() {
  std::vector<int> const vi{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto rng = vi | std::views::filter([](int i){return i % 2 == 1;})
                | std::views::transform([](int i){return std::to_string(i);});

  std::ranges::for_each(rng, [](std::string s){std::cout << s << std::endl;});

  return 0;
}



