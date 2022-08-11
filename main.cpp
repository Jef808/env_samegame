#include "action_list.h"
#include "agent.h"
#include "samegame.h"
#include "viewer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

enum class Agent { Random, Greedy, ColorCount };

using namespace std;

constexpr size_t WIDTH = 15;
constexpr size_t HEIGHT = 15;

typedef std::vector< std::tuple< double, double, double > > Results;

void print_scores(const Results& results){
  auto t = 1;
  for (auto [score_random, score_greedy, score_ccount] : results) {
    cout << "\n\nTest " << t++
         << ":\n  AgentRandom: " << score_random
         << ",\n  AgentGreedy: " << score_greedy
         << ",\n  AgentColorCount: " << score_ccount << endl;
  }
}

template <typename AgentT> double run(std::ifstream &ifs, bool enable_viewer = false) {
  SameGame sg{WIDTH, HEIGHT};
  sg.load(ifs);

  if(enable_viewer)
    Viewer::print(cout, sg);

  AgentT agent;
  double score = 0.0;

  while (true) {
    auto [okay, action] = agent.choose(sg);

    if (not okay) {
      break;
    }

    score += sg.score(action);
    sg.apply(action);

    if(enable_viewer) {
      Viewer::print(cout, sg);
      cout << "\nScore: " << score << endl;
    }
  }

  if(enable_viewer) {
    Viewer::print(cout, sg);
    cout << "\n\nScore: " << score << endl;
  }

  return score;
}



int main(int argc, char *argv[]) {
  // if (argc < 2) {
  //   cerr << "USAGE: " << argv[0] << " INPUT FILE" << endl;
  //   return EXIT_FAILURE;
  // }
  // std::string input_fn = DATA_DIR + std::string(argv[1]);

  Results scores;

  for(auto t = 1; t < 51; ++t) {

    //auto& [score_random, score_greedy, score_ccount] = scores.emplace_back();
    auto& scores_vec = scores.emplace_back();

    std::string input_fn = DATA_DIR + std::string("test") + std::to_string(t) + ".txt";

    for (auto i : { 0, 1, 2 }) {
      ifstream ifs{input_fn};
      if (not ifs) {
        cerr << "Failed to open input file " << input_fn << endl;
        return EXIT_FAILURE;
      }

      double& score = i == 0? get< 0 >(scores_vec)
        : i == 1? get< 1 >(scores_vec): get< 2 >(scores_vec);

      score = i == 0? run< AgentRandom >(ifs)
        : i == 1? run< AgentGreedy >(ifs): run< AgentLowColorCount >(ifs);
    }
  }

  print_scores(scores);

  double sum_random = 0.0;
  double sum_greedy = 0.0;
  double sum_ccount = 0.0;

  auto t = 1;
  for (const auto& [score_random, score_greedy, score_ccount] : scores) {
    sum_random += score_random;
    sum_greedy += score_greedy;
    sum_ccount += score_ccount;
  }

  cout << "\n\n\nAverages:\n  AgentRandom: " << sum_random / scores.size()
       << "\n  AgentGreedy: " << sum_greedy / scores.size()
       << "\n  AgentColorCount: " << sum_ccount / scores.size() << endl;

  return EXIT_SUCCESS;
}

