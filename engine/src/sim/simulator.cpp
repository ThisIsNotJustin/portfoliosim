#include "simulator.hpp"

#include <future>
#include <algorithm>
#include <cmath>

Simulator::Simulator(const std::vector<Asset>& assets, 
        const std::vector<double>& weights,
        double initial,
        double monthly,
        unsigned int trials,
        unsigned int years) :
    assets_(assets),
    weights_(weights),
    trials_(trials),
    years_(years),
    monthly_(monthly),
    initial_(initial),
    rng_(std::random_device{}()) {}

Simulator::Simulator(const std::vector<Asset>& assets, 
        const std::vector<double>& weights,
        std::vector<double> initial,
        std::vector<double> monthly,
        unsigned int trials,
        std::vector<unsigned int> years) :
    assets_(assets),
    weights_(weights),
    trials_(trials),
    years_vec_(years),
    monthly_vec_(monthly),
    initial_vec_(initial),
    rng_(std::random_device{}()) {}

Simulator::Path Simulator::simulatePath(size_t asset_idx) const {
    unsigned int years = years_vec_.empty() ? years_ : years_vec_[asset_idx];
    double monthly = monthly_vec_.empty() ? monthly_ : monthly_vec_[asset_idx];
    double val = initial_vec_.empty() ? initial_ : initial_vec_[asset_idx];

    Path path;
    path.reserve(years);

    unsigned int months = years * 12;
    const auto& log_returns = assets_[asset_idx].getReturns();
    std::uniform_int_distribution<> dayDist(0, log_returns.size() - 1);

    for (unsigned int m = 0; m < months; m++) {
        double sum_logs = 0.0;
        for (unsigned int d = 0; d < 21; d++) {
            sum_logs += log_returns[dayDist(rng_)];
        }

        double monthlyFactor = std::exp(sum_logs);
        val = val * monthlyFactor + monthly;
        
        if ((m + 1) % 12 == 0) {
            path.push_back(val);
        }
    }

    return path;
}

Simulator::Results Simulator::runSingleThread() const {
    Results all;
    all.reserve(trials_);

    if (!initial_vec_.empty()) {
        for (size_t asset_idx = 0; asset_idx < initial_vec_.size(); asset_idx++) {
            for (unsigned int t = 0; t < trials_; t++) {
                all.push_back(simulatePath(asset_idx));
            }
        }
    } else {
        for (unsigned int t = 0; t < trials_; t++) {
            all.push_back(simulatePath(0));
        }
    }

    return all;
}

Simulator::Results Simulator::runMultiThread() const {
    Results all;
    all.reserve(trials_);
    std::vector<std::future<Path>> futures;
    futures.reserve(trials_);

    if (!initial_vec_.empty()) {
        for (size_t asset_idx = 0; asset_idx < initial_vec_.size(); asset_idx++) {
            for (unsigned int t = 0; t < trials_; t++) {
                futures.emplace_back(
                    std::async(std::launch::async, [this, asset_idx]() {
                        return simulatePath(asset_idx);
                    })
                );
            }
        }
    } else {
        for (unsigned int t = 0; t < trials_; t++) {
            futures.emplace_back(
                std::async(std::launch::async, [this]() {
                    return simulatePath(0);
                })
            );
        }
    }

    for (auto& future : futures) {
        all.push_back(future.get());
    }

    return all;
}
