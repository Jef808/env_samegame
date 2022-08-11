#include "agent.h"
#include "samegame.h"
#include "viewer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

constexpr size_t WIDTH = 15;
constexpr size_t HEIGHT = 15;

SameGame sg{WIDTH, HEIGHT};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "USAGE: " << argv[0] << " INPUT FILE" << endl;
    return EXIT_FAILURE;
  }
  std::string input_fn = DATA_DIR + std::string(argv[1]);

  ifstream ifs{input_fn};
  if (not ifs) {
    cerr << "Failed to open input file " << input_fn << endl;
    return EXIT_FAILURE;
  }

  sg.load(ifs);
  Viewer::print(cout, sg);

  AgentRandom agent;
  bool okay = true;

  while (true) {
    Action action;

    std::tie(okay, action) = agent.choose(sg);

    if (not okay) {
      break;
    }

    sg.apply(action);

    Viewer::print(cout, sg);
  }

  Viewer::print(cout, sg);

  return EXIT_SUCCESS;
}
