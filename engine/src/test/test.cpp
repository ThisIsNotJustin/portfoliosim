/*
Simple Monte Carlo pi estimation


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

int main() {
    const int NUM_POINTS = 100000;
    int inside = 0;

    std::srand(static_cast<unsigned>(std::time(0)));
    for (int i = 0; i < NUM_POINTS; i++) {
        double x = static_cast<double>(std::rand()) / RAND_MAX * 2.0 - 1.0;
        double y = static_cast<double>(std::rand()) / RAND_MAX * 2.0 - 1.0;

        if (x*x + y*y <= 1.0) {
            inside++;
        }
    }

    double est = 4.0 * inside / NUM_POINTS;
    std::cout << "Estimated pi: " << est << "\n";

    return 0;
}

*/

#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>

const int NUM_POINTS = 1000000;
const int NUM_THREADS = 4;

std::mutex mut;
int total = 0;

void monte_carlo(int ppt) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    int inside = 0;
    for (int i = 0; i < ppt; i++) {
        double x = dis(gen);
        double y = dis(gen);

        if (x*x + y*y <= 1) {
            inside++;
        }
    }

    std::lock_guard<std::mutex> lock(mut);
    total += inside;
}

int main() {
    int ppt = NUM_POINTS / NUM_THREADS;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(monte_carlo, ppt);
    }

    for (auto& t : threads) {
        t.join();
    }

    double est = 4.0 * total / NUM_POINTS;
    std::cout << "Estimated pi: " << est << "\n";

    return 0;
}