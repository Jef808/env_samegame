#include "action_list.h"
#include "agent.h"
#include "samegame.h"
#include "viewer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

enum class Agent { Random, Greedy };

using namespace std;

constexpr size_t WIDTH = 15;
constexpr size_t HEIGHT = 15;

std::vector< std::pair< double, double > > scores;

void print_scores(const auto &scores){

  auto t = 1;
  for (auto [score_random, score_greedy] : scores) {
    cout << "\n\nTest " << t++ << ":\n"
      << "  AgentRandom: " << score_random << "\n  AgentGreedy: " << score_greedy << endl;
  }
}


template <Agent A> double run(std::ifstream &ifs, bool enable_viewer = false) {
  SameGame sg{WIDTH, HEIGHT};
  sg.load(ifs);

  if(enable_viewer)
    Viewer::print(cout, sg);

  using Agent_t =
      std::conditional_t<A == Agent::Random, AgentRandom, AgentGreedy>;
  Agent_t agent;

  bool okay = true;

  double score = 0.0;

  while (true) {
    Action action;

    std::tie(okay, action) = agent.choose(sg);

    if (not okay) {
      break;
    }

    score += sg.score(action);

    sg.apply(action);

    if(enable_viewer)
      Viewer::print(cout, sg);

    //cout << "\nScore: " << score << endl;
  }

  if(enable_viewer)
    Viewer::print(cout, sg);

  //cout << "\n\nScore: " << score << endl;

  return score;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "USAGE: " << argv[0] << " INPUT FILE" << endl;
    return EXIT_FAILURE;
  }
  std::string input_fn = DATA_DIR + std::string(argv[1]);


  for(auto t = 1; t < 51; ++t) {

    auto& [score_random, score_greedy] = scores.emplace_back();

    std::string input_fn = DATA_DIR + std::string("test") + std::to_string(t) + ".txt";

    for (auto i : { 0, 1 }) {
      ifstream ifs{input_fn};
      if (not ifs) {
        cerr << "Failed to open input file " << input_fn << endl;
        return EXIT_FAILURE;
      }

      double& score = i == 0? score_random: score_greedy;
      score = i == 0? run< Agent::Random >(ifs): run< Agent::Greedy >(ifs);
    }
  }

  double tr = 0.0, tg = 0.0;

  print_scores(scores);

  auto t = 1;
  for (auto [score_random, score_greedy] : scores) {
    tr += score_random;
    tg += score_greedy;
  }

  cout << "\n\n\nAverages:\n  AgentRandom: " << tr / scores.size()
    << "\n  AgentGreedy: " << tg / scores.size() << endl;

  return EXIT_SUCCESS;
}

