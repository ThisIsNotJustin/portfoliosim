#include "data_loader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

TimeSeries DataLoader::loadPrices(const std::string& path) {
    TimeSeries prices;
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Error opening file path: " << path << "\n";
        return prices;
    }

    std::string line;
    std::getline(in, line);

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::vector<std::string> parts;
        std::string token, date, close_string;

        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }

        if (parts.size() < 5) continue;

        date = parts[0];
        close_string = parts[4];
        try {
            double close = std::stod(close_string);
            prices.push_back(close);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing close price: " << close_string << " line: " << line << "\n";
        }
    }

    std::reverse(prices.begin(), prices.end());
    return prices;
}

TimeSeries DataLoader::computeReturns(const TimeSeries& prices) {
    TimeSeries logRets;
    if (prices.size() < 2) {
        return logRets;
    }
    
    logRets.reserve(prices.size() - 1);
    for (size_t i = 1; i < prices.size(); ++i) {
        logRets.push_back(std::log(prices[i] / prices[i-1]));
    }
    return logRets;
}