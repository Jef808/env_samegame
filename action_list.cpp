#include "action_list.h"

#include <algorithm>
#include <limits>
#include <random>


namespace {
    std::vector<uint64_t> Key;
}

ActionSequence::ActionSequence(SameGame& sg, size_t n_actions)
    : m_sg{ sg }
{
    Key.reserve(n_actions);

    std::mt19937_64 gen{ std::random_device{}() };
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits< uint64_t >::max());

    std::generate_n(std::back_inserter(Key), n_actions, [&gen, &dist](){
        return dist(gen);
    });
}

void ActionSequence::add(const Action& action) {
    m_actions.push_back(action);
    m_hash ^= Key[action.index];

    p_data->score += m_scores.emplace_back(m_sg.score(action));

    m_sg.apply(action);
}

void ActionSequence::remove() {
    if (not m_actions.empty()) {
        m_hash ^= Key[m_actions.back().index];
        m_actions.pop_back();
    }
}
