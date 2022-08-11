#ifndef ACTION_LIST_H_
#define ACTION_LIST_H_

#include "samegame.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_map>

struct Action;

struct ActionSequenceData {
    double score;
    double value;
    int visits;
};

class ActionSequence {
public:
    ActionSequence(SameGame& sg, size_t n_actions);

    void add(const Action& action);
    void remove();

private:
    SameGame& m_sg;
    std::deque<Action> m_actions;
    std::deque<double> m_scores;
    uint64_t m_hash;

    ActionSequenceData* p_data;
};

extern std::unordered_map< uint64_t, ActionSequenceData > TTable;

#endif // ACTION_LIST_H_
