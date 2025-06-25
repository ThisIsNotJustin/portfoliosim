#include "standard.hpp"

#include <cmath>

Standard::Standard(int initial, int monthly, int years) {

}

std::vector<double> Standard::RunSimple(int initial, int monthly, int years) {
    std::vector<double> results;
    double balance = initial;
    double monthly_rate = std::pow(1.10, 1.0/12.0) - 1.0;

    for (int y = 0; y < years; y++) {
        for (int m = 0; m < 12; m++) {
            balance = balance * (1.0 + monthly_rate) + monthly;
        }

        results.push_back(balance);
    }

    return results;
}