#pragma once

#include <string>
#include <vector>
#include <map>

using TimeSeries = std::vector<double>;

class DataLoader {
    public:
        static TimeSeries loadPrices(const std::string& path);

        static TimeSeries computeReturns(const TimeSeries& prices);
};