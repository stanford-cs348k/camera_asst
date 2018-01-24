#include "common.hpp"

template<> int Random::UniformRandom<int>(const int& a, const int& b) {
  return std::uniform_int_distribution<>(a, b)(generator_);
}

template<> float Random::UniformRandom<float>(const float& a, const float& b) {
  return std::uniform_real_distribution<float>(a, b)(generator_);
}

template<> double Random::UniformRandom<double>(
    const double& a, const double& b) {
  return std::uniform_real_distribution<double>(a, b)(generator_);
}
