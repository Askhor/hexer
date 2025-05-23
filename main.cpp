#include "tclap/CmdLine.h"
#include <iostream>

using namespace TCLAP;

int main(int argc, char **argv) {
    CmdLine cmd("Test command");
    cmd.parse(argc, argv);

    std::cout << "Hello, World!" << std::endl;
}
