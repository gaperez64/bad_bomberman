#include "logic/Random.h"

namespace logic {

Random& Random::instance() {
    static Random inst;
    return inst;
}

Random::Random() : m_engine{std::random_device{}()} {}

int Random::getInt(int min, int max) {
    std::uniform_int_distribution<int> dist{min, max};
    return dist(m_engine);
}

float Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist{min, max};
    return dist(m_engine);
}

bool Random::getBool(float probability) {
    return getFloat(0.f, 1.f) < probability;
}

}  // namespace logic
