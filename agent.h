#ifndef AGENT_H_
#define AGENT_H_

#include <random>
#include <vector>

#include "action_list.h"
#include "samegame.h"

class AgentRandom {
public:
  AgentRandom() = default;

  std::pair<bool, Action> choose(const SameGame &sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
  std::mt19937 gen{std::random_device{}()};
};

class AgentGreedy {
public:
  AgentGreedy() = default;

  std::pair<bool, Action> choose(const SameGame& sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
};

class AgentLowColorCount {
public:
  AgentLowColorCount() = default;

  std::pair<bool, Action> choose(const SameGame& sg);

private:
  std::vector<Action> m_buffer;
  std::vector<double> m_scores;
  std::array<int, NB_COLORS> m_ccounter;
};


#endif // AGENT_H_
