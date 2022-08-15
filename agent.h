#ifndef AGENT_H_
#define AGENT_H_

#include "types.h"

#include <iosfwd>

template <typename SelectionPolicy>
double run(std::istream& ifs, bool enable_viewer = false);


#include "agent.hpp"
#endif // AGENT_H_
