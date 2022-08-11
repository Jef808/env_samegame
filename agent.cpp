#include "agent.h"

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
