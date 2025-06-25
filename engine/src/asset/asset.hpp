#pragma once

#include <string>
#include <vector>

class Asset {
    public:
        Asset(std::string ticker);

        void setReturns(const std::vector<double>& returns);

        const std::string& getTicker() const;
        const std::vector<double>& getReturns() const;
        double meanReturn() const;
        double volatility() const;

    private:
        std::string ticker_;
        std::vector<double> returns_;
        mutable bool statsComputed_ = false;
        mutable double meanCache_ = 0.0;
        mutable double volCache_ = 0.0;

        void computeStats() const;
};