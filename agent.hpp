#ifndef AGENT_HPP_
#define AGENT_HPP_

#include "samegame.h"
#include "viewer.h"

#include <iostream>

template <typename SelectionPolicy> double run(std::istream &ifs, bool enable_viewer) {
  SameGame sg{WIDTH, HEIGHT};
  sg.load(ifs);

  if(enable_viewer)
    Viewer::print(std::cout, sg);

  SelectionPolicy selection_policy;
  double score = 0.0;

  while (true) {
    auto [okay, action] = selection_policy(sg);

    if (not okay) {
      break;
    }

    score += sg.score(action);
    sg.apply(action);

    if(enable_viewer) {
      Viewer::print(std::cout, sg);
      std::cout << "\nScore: " << score << std::endl;
    }
  }

  if(enable_viewer) {
    Viewer::print(std::cout, sg);
    std::cout << "\n\nScore: " << score << std::endl;
  }

  return score;
}


#endif // AGENT_HPP_
