#pragma once

#include <vector>

class Standard {
    public:
        Standard(int initial, int monthly, int years);

        std::vector<double> RunSimple(int initial, int monthly, int years);
};