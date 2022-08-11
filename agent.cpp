#include "agent.h"

#include <algorithm>
#include <cassert>
#include <random>

std::pair<bool, Action> AgentRandom::choose(const SameGame &sg) {
  m_buffer.clear();
  sg.valid_actions(std::back_inserter(m_buffer));

  if (m_buffer.empty()) {
    return std::make_pair(false, Action{-1});
  }

  return std::make_pair(
      true,
      m_buffer[std::uniform_int_distribution<>(0, m_buffer.size() - 1)(gen)]);
}

std::pair<bool, Action> AgentGreedy::choose(const SameGame &sg) {
  m_buffer.clear();
  sg.valid_actions(std::back_inserter(m_buffer));

  if (m_buffer.empty()) {
    return std::make_pair(false, Action{-1});
  }

  return std::make_pair(true,
                        *std::max_element(m_buffer.begin(), m_buffer.end(),
                                          [&sg](const auto &a, const auto &b) {
                                            return sg.score(a) < sg.score(b);
                                          }));
}
