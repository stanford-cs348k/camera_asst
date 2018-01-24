#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <vector>

template<typename T>
T Clamp(const T& val, const T& min, const T& max) {
  return val > min ? (val < max ? val : max) : min;
}

template<typename T>
T Sign(const T& val) {
  return val <= (T)0. ? (T)-1. : (T)1.;
}

class Random {
 public:
  using GeneratorType = std::mt19937;
  using ResultType = GeneratorType::result_type;

  Random() : generator_() {}
  explicit Random(ResultType seed) : generator_(seed) {}

  void Reseed() { generator_.seed(std::random_device()()); }
  void Seed(ResultType seed) { generator_.seed(seed); }

  template<typename T> T UniformRandom(const T& a, const T& b);

 private:
  GeneratorType generator_;
};

template<typename K, typename V>
V GetOrDefault(const std::map<K, V>& map, const K& key, const V& val) {
  if (map.find(key) == map.end()) return val;
  return map.at(key);
}

template<typename Container, typename Key>
bool In(const Container& c, const Key& k) {
  return c.find(k) != c.end();
}

class ArgParser {
 public:
  ArgParser(int argc, char** argv) {
    for (int i = 0; i < argc; i++)
      args_.push_back(std::string(argv[i]));
  }

  bool HasArg(std::string arg) const {
    auto it = std::find(args_.begin(), args_.end(), arg);
    if (it == args_.end()) return false;
    return true;
  }

  std::string GetArg(std::string arg) const {
    auto it = std::find(args_.begin(), args_.end(), arg);
    if (it == args_.end() || (++it) == args_.end()) return "";
    return *it;
  }

 private:
  std::vector<std::string> args_;
};

#endif  // COMMON_HPP_
