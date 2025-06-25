#include <iostream>
#include <cstdlib>
#include "standard.hpp"

int main2(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: standard_cli <initial> <monthly> <years>\n";
        return 1;
    }

    int initial = std::atoi(argv[1]);
    int monthly = std::atoi(argv[2]);
    int years = std::atoi(argv[3]);

    Standard stdsim(initial, monthly, years);
    auto results = stdsim.RunSimple(initial, monthly, years);

    for (auto r : results) {
        std::cout << r << std::endl;
    }

    return 0;
}