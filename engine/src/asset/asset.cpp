#include "asset.hpp"

#include <numeric>
#include <cmath>

Asset::Asset(std::string ticker): ticker_(std::move(ticker)){}

void Asset::setReturns(const std::vector<double>& returns) {
    returns_ = returns;
    statsComputed_ = false;
}

const std::string& Asset::getTicker() const {
    return ticker_;
}

const std::vector<double>& Asset::getReturns() const {
    return returns_;
}

double Asset::meanReturn() const {
    if (!statsComputed_) computeStats();

    return meanCache_;
}

double Asset::volatility() const {
    if (!statsComputed_) computeStats();

    return volCache_;
}

void Asset::computeStats() const {
    if (returns_.empty()) {
        meanCache_ = volCache_ = 0.0;
    } else {
        double sum = std::accumulate(returns_.begin(), returns_.end(), 0.0);
        meanCache_ = sum / returns_.size();
        double accum = 0.0;
        for (double r : returns_) {
            accum += (r - meanCache_) * (r - meanCache_);
        }

        volCache_ = std::sqrt(accum / returns_.size());
    }

    statsComputed_ = true;
}