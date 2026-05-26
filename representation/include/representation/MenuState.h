#pragma once

#include <array>
#include <memory>
#include <SFML/Graphics.hpp>
#include "State.h"
#include "logic/Score.h"

namespace representation {

class MenuState : public State {
public:
    MenuState(StateManager& manager, std::shared_ptr<sf::RenderWindow> window,
              logic::Score& score);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    void startGame();
    std::shared_ptr<sf::RenderWindow> m_window;
    logic::Score& m_score;

    sf::Font m_font;
    bool m_fontLoaded{false};

    sf::RectangleShape m_playButton;
    sf::Text m_titleText;
    sf::Text m_playText;
    sf::Text m_scoreTitle;
    std::array<sf::Text, 5> m_scoreTexts;

    sf::FloatRect m_playButtonBounds;
    bool m_playHovered{false};
};

}  // namespace representation
