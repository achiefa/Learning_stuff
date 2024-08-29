#include "utils_distribution.hpp"

#ifndef INCLUDE_DISTRIBUTION_POLICIES
#define INCLUDE_DISTRIBUTION_POLICIES

template <typename RNG>
requires std::uniform_random_bit_generator<RNG>
class Gaussian
{ 
  public:
  Gaussian(RNG* generator,
           const double& mean,
           const double& var)
  : mean_(mean), var_(var), generator_(generator)
  { 
    distribution_ = std::normal_distribution<double> (mean_, var_);
  }

  Gaussian(RNG* generator)
  : Gaussian(generator, 0.0, 1.0)
  { }


  const double operator()() const
  {
    return distribution_(*generator_);
  }

  RNG* generator_;
  double mean_, var_;
  // Somehow, mutable is needed if I want to use a const method
  mutable std::normal_distribution<double> distribution_;
};


template <typename RNG>
requires std::uniform_random_bit_generator<RNG>
class Unifrom
{ 
  public:
  Unifrom(RNG* generator,
           const double& max,
           const double& min)
  : max_(max), min_(min), generator_(generator)
  { 
    distribution_ = std::uniform_real_distribution<double> (min_, max_);
  }

  Unifrom(RNG* generator)
  : Unifrom(generator, -1.0, 1.0)
  { }


  const double operator()() const
  {
    return distribution_(*generator_);
  }

  RNG* generator_;
  double max_, min_;
  // Somehow, mutable is needed if I want to use a const method
  mutable std::uniform_real_distribution<double> distribution_;
};


// TODO: implement API for custom distribution
template <typename RNG>
requires std::uniform_random_bit_generator<RNG>
class custom_distribution
{

};

#endif