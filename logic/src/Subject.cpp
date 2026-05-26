#include "logic/Subject.h"

#include <algorithm>

namespace logic {

void Subject::addObserver(Observer* observer) {
    m_observers.push_back(observer);
}

void Subject::removeObserver(Observer* observer) {
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer),
                      m_observers.end());
}

void Subject::notify(const Event& event) {
    for (auto* obs : m_observers) {
        obs->onNotify(event);
    }
}

}  // namespace logic
