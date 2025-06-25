#pragma once

#include <vector>
#include <random>

#include "../asset/asset.hpp"

class Simulator {
    public:
        using Path = std::vector<double>;
        using Results = std::vector<Path>;

        Simulator(const std::vector<Asset>& assets,
                    const std::vector<double>& weights,
                    double initial,
                    double monthly,
                    unsigned int trials,
                    unsigned int years);

        Simulator(const std::vector<Asset>& assets,
                    const std::vector<double>& weights,
                    std::vector<double> initial,
                    std::vector<double> monthly,
                    unsigned int trials,
                    std::vector<unsigned int> years);
        
        Results runSingleThread() const;
        Results runMultiThread() const;

    private:
        const std::vector<Asset>& assets_;
        std::vector<double> weights_;
        unsigned int trials_;
        unsigned int years_;
        std::vector<unsigned int> years_vec_;
        double monthly_;
        std::vector<double> monthly_vec_;
        double initial_;
        std::vector<double> initial_vec_;
        mutable std::mt19937 rng_;

        Path simulatePath(size_t asset_idx) const;
};