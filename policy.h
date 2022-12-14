#ifndef POLICY_H_
#define POLICY_H_

#include <random>
#include <vector>

#include "samegame.h"

class PolicyRandom {
public:
  PolicyRandom() = default;

  std::pair<bool, Action> operator()(const SameGame &sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
  std::mt19937 gen{std::random_device{}()};
};

class PolicyGreedy {
public:
  PolicyGreedy() = default;

  std::pair<bool, Action> operator()(const SameGame& sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
};

class PolicyLowColorCount {
public:
  PolicyLowColorCount() = default;

  std::pair<bool, Action> operator()(const SameGame& sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
  std::array<int, NB_COLORS> m_ccounter;
};


#endif // AGENT_H_
