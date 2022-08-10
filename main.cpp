#include "viewer.h"
#include "samegame.h"

#include <iostream>
#include <string>
#include <fstream>

using namespace std;


constexpr size_t WIDTH = 15;
constexpr size_t HEIGHT = 15;


SameGame sg{ WIDTH, HEIGHT };

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "USAGE: " << argv[0] << " INPUT FILE" << endl;
        return EXIT_FAILURE;
    }
    std::string input_fn = DATA_DIR + std::string(argv[1]);

    ifstream ifs{ input_fn };
    if (not ifs) {
        cerr << "Failed to open input file " << input_fn << endl;
        return EXIT_FAILURE;
    }

    sg.load(ifs);

    Viewer::print(cout, sg);

    return EXIT_SUCCESS;
}
