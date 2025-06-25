#include "src/asset/asset.hpp"
#include "src/loading/data_loader.hpp"
#include "src/sim/simulator.hpp"

#include <iostream>

int main() {
    TimeSeries apple_price = DataLoader::loadPrices("../data/AAPL_av_history.csv");
    if (apple_price.empty()) {
        std::cerr << "Couldn't load apple prices\n"; 
        return 1;
    }

    TimeSeries returns1 = DataLoader::computeReturns(apple_price);
    Asset apple("AAPL");
    apple.setReturns(returns1);
    std::cout << "AAPL mean return: " << apple.meanReturn() * 252 << "\n";
    std::cout << "AAPL volatility: " << apple.volatility() * 252 << "\n";

    std::vector<Asset> assets = {apple};
    std::vector<double> weights = {1.0};
    Simulator sim(assets, weights, 10000.0, 1000.0, 10000, 10);
    auto results = sim.runMultiThread();

    double sumFinal = 0.0;
    for (auto& path : results) {
        sumFinal += path.back();
    }

    double avg = sumFinal / results.size();
    std::cout << "Average Apple only Portfolio Value: " << avg << "\n";

    TimeSeries amazon_price = DataLoader::loadPrices("../data/AMZN_av_history.csv");
    if (amazon_price.empty()) {
        std::cerr << "Couldn't load amazon prices\n";
        return 1;
    }

    TimeSeries returns2 = DataLoader::computeReturns(amazon_price);
    Asset amazon("AMZN");
    amazon.setReturns(returns2);
    std::cout << "AMZN mean return: " << amazon.meanReturn() * 252 << "\n";
    std::cout << "AMZN volatility: " << amazon.volatility() * 252 << "\n";
    assets = {amazon};
    Simulator sim2(assets, weights, 10000.0, 1000.0, 10000, 10);
    auto results2 = sim2.runMultiThread();

    sumFinal = 0.0;
    for (auto& path : results2) {
        sumFinal += path.back();
    }

    avg = sumFinal / results2.size();
    std::cout << "Average Amazon only Portfolio Value: " << avg << "\n";

    return 0;
}