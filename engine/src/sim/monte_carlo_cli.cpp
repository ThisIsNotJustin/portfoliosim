#include "../asset/asset.hpp"
#include "../loading/data_loader.hpp"
#include "simulator.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    json params = json::parse(input);

    auto allocations = params["allocations"];
    std::vector<double> initial;
    std::vector<double> monthly;
    std::vector<unsigned int> years;
    unsigned int trials = 10000;

    std::vector<Asset> assets;
    std::vector<double> weights;

    for (auto a = allocations.begin(); a != allocations.end(); a++) {
        std::string symbol = a.key();
        auto alloc = a.value();
        double weight = 1.0;

        double initial_val = alloc.value("initial", 0.0);
        double monthly_val = alloc.value("monthly", 0.0);
        unsigned int years_val = alloc.value("years", 0);
        initial.push_back(initial_val);
        monthly.push_back(monthly_val);
        years.push_back(years_val);

        std::string csvPath = "../../data/" + symbol + "_av_history.csv";
        std::cerr << "Trying to load: " << std::filesystem::absolute(csvPath) << std::endl;
        std::ifstream test(csvPath);
        if (!test) {
            std::cerr << "File does not exist or cannot be opened: " << csvPath << std::endl;
            std::cout << "No File to open" << std::endl;
            continue;
        }

        TimeSeries prices = DataLoader::loadPrices(csvPath);
        if (prices.empty()) {
            std::cerr << "No prices for " << symbol << "\n";
            continue;
        }
        TimeSeries returns = DataLoader::computeReturns(prices);
        Asset asset(symbol);
        asset.setReturns(returns);
        assets.push_back(asset);
        weights.push_back(weight);
    }

    if (assets.empty()) {
        std::cerr << "No valid assets loaded.\n";
        std::cout << "{\"error\": \"No valid assets loaded.\"}" << std::endl;
        return 1;
    }

    Simulator sim(assets, weights, initial, monthly, trials, years);
    auto results = sim.runMultiThread();

    std::vector<std::vector<double>> avgPaths(assets.size());
    for (size_t asset_idx = 0; asset_idx < assets.size(); asset_idx++) {
        avgPaths[asset_idx].resize(years[asset_idx], 0.0);
    }

    size_t result_idx = 0;
    for (size_t asset_idx = 0; asset_idx < assets.size(); asset_idx++) {
        for (unsigned int t = 0; t < trials; t++) {
            const auto& path = results[result_idx++];
            for (size_t i = 0; i < path.size(); i++) {
                avgPaths[asset_idx][i] += path[i];
            }
        }
    }
    for (size_t asset_idx = 0; asset_idx < assets.size(); asset_idx++) {
        for (double& v : avgPaths[asset_idx]) {
            v /= trials;   
        }
    }

    json out;
    for (size_t asset_idx = 0; asset_idx < assets.size(); asset_idx++) {
        out["values"][assets[asset_idx].getTicker()] = avgPaths[asset_idx];
        out["projectedValue"][assets[asset_idx].getTicker()] = avgPaths[asset_idx].back();
    }
    std::cout << out.dump() << std::endl;

    return 0;
}