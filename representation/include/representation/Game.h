#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "StateManager.h"
#include "logic/Score.h"

namespace representation {

class Game {
public:
    Game();
    void run();

private:
    static constexpr unsigned WINDOW_WIDTH  = 832;   // 13 tiles × 64 px
    static constexpr unsigned WINDOW_HEIGHT = 704;   // 11 tiles × 64 px
    static constexpr unsigned MAX_FPS       = 60;

    void scaleMouseEvent(sf::Event& event) const;

    std::shared_ptr<sf::RenderWindow> m_window;
    StateManager m_stateManager;
    logic::Score m_score;
    float m_hiDPIScale{1.f};
};

}  // namespace representation
