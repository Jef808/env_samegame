#include "agent.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <random>

std::pair<bool, Action> PolicyRandom::choose(const SameGame &sg) {
  m_buffer.clear();
  sg.valid_actions(std::back_inserter(m_buffer));

  if (m_buffer.empty()) {
    return std::make_pair(false, Action{-1});
  }

  return std::make_pair(
      true,
      m_buffer[std::uniform_int_distribution<>(0, m_buffer.size() - 1)(gen)]);
}

std::pair<bool, Action> PolicyGreedy::choose(const SameGame &sg) {
  m_buffer.clear();
  sg.valid_actions(std::back_inserter(m_buffer));

  if (m_buffer.empty()) {
    return std::make_pair(false, Action{-1});
  }

  auto Cmp = [&sg](const auto &a, const auto &b) {
    return sg.score(a) < sg.score(b);
  };

  return std::make_pair(
      true, *std::max_element(m_buffer.begin(), m_buffer.end(), Cmp));
}

std::pair<bool, Action> PolicyLowColorCount::choose(const SameGame &sg) {
  m_buffer.clear();
  sg.valid_actions(std::back_inserter(m_buffer));

  if (m_buffer.empty()) {
    return std::make_pair(false, Action{-1});
  }

  auto Cmp = [&sg](const auto &a, const auto &b) {
    const Color a_color = sg.get_color(a.index);
    const Color b_color = sg.get_color(b.index);
    return sg.get_color_count(a_color) < sg.get_color_count(b_color);
  };

  return std::make_pair(
      true, *std::min_element(m_buffer.begin(), m_buffer.end(), Cmp));
}
