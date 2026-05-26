#include "logic/Score.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace logic {

Score::Score(const std::string& scoresFile) : m_scoresFile{scoresFile} {
    loadScores();
}

void Score::onNotify(const Event& event) {
    switch (event.type) {
        case EventType::BlockDestroyed:  m_current += 10;  break;
        case EventType::PowerUpPickedUp: m_current += 50;  break;
        case EventType::EnemyKilled:     m_current += 200; break;
        case EventType::PlayerWon:       m_current += 1000; break;
        case EventType::PlayerLost:      m_current = std::max(0, m_current - 500); break;
        default: break;
    }
}

void Score::update(float dt) {
    m_timeAccum += dt;
    if (m_timeAccum >= 1.f) {
        m_current += static_cast<int>(m_timeAccum);
        m_timeAccum -= static_cast<int>(m_timeAccum);
    }
}

void Score::saveScore() {
    // Insert current score into top-5
    m_topScores[4] = m_current;
    std::sort(m_topScores.begin(), m_topScores.end(), std::greater<int>{});
    persistScores();
}

void Score::reset() {
    m_current = 0;
    m_timeAccum = 0.f;
}

void Score::loadScores() {
    m_topScores.fill(0);
    std::ifstream file{m_scoresFile};
    if (!file.is_open()) return;  // first run — no file yet

    int idx = 0;
    int val = 0;
    while (idx < 5 && file >> val) {
        m_topScores[idx++] = val;
    }
}

void Score::persistScores() const {
    std::ofstream file{m_scoresFile};
    if (!file.is_open()) return;  // non-fatal, scores just won't persist
    for (int s : m_topScores) {
        file << s << "\n";
    }
}

}  // namespace logic
