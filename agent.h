#ifndef AGENT_H_
#define AGENT_H_

#include <random>
#include <vector>

#include "samegame.h"

class AgentRandom {
public:
    AgentRandom() = default;

    std::pair< bool, Action > choose(const SameGame& sg);

private:
    std::vector< Action > m_buffer;
    std::mt19937 gen{ std::random_device{}() };
};

#endif // AGENT_H_
