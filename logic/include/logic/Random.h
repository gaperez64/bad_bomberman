#pragma once

#include <random>

namespace logic {

class Random {
public:
    static Random& instance();

    int getInt(int min, int max);
    float getFloat(float min, float max);
    bool getBool(float probability);  // true with given probability [0,1]

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

private:
    Random();

    std::mt19937 m_engine;
};

}  // namespace logic
