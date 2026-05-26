#pragma once

#include <array>
#include <string>
#include "Observer.h"

namespace logic {

class Score : public Observer {
public:
    explicit Score(const std::string& scoresFile = "scores.txt");

    void onNotify(const Event& event) override;
    void update(float dt);  // adds time-alive points

    int getCurrent() const { return m_current; }

    /// Returns sorted top-5 scores (highest first)
    std::array<int, 5> getTopScores() const { return m_topScores; }

    void saveScore();        // adds current to top-5 and persists
    void reset();

private:
    void loadScores();
    void persistScores() const;

    std::string m_scoresFile;
    int m_current{0};
    float m_timeAccum{0.f};
    std::array<int, 5> m_topScores{};
};

}  // namespace logic
