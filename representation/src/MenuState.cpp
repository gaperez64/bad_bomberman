#include "representation/MenuState.h"

#include <iostream>
#include <string>
#include "representation/GameState.h"
#include "representation/StateManager.h"

namespace representation {

MenuState::MenuState(StateManager& manager, std::shared_ptr<sf::RenderWindow> window,
                     logic::Score& score)
    : State{manager}, m_window{std::move(window)}, m_score{score} {
    m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/liberation-sans-fonts/LiberationSans-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("assets/font.ttf");

    unsigned w = m_window->getSize().x;
    unsigned h = m_window->getSize().y;

    if (m_fontLoaded) {
        m_titleText.setFont(m_font);
        m_titleText.setString("BOMBERMAN");
        m_titleText.setCharacterSize(60);
        m_titleText.setFillColor(sf::Color::Yellow);
        auto tb = m_titleText.getLocalBounds();
        m_titleText.setPosition(w * 0.5f - tb.width * 0.5f, 60.f);

        m_scoreTitle.setFont(m_font);
        m_scoreTitle.setString("TOP SCORES");
        m_scoreTitle.setCharacterSize(30);
        m_scoreTitle.setFillColor(sf::Color::Cyan);
        auto sb = m_scoreTitle.getLocalBounds();
        m_scoreTitle.setPosition(w * 0.5f - sb.width * 0.5f, 160.f);

        auto scores = m_score.getTopScores();
        for (int i = 0; i < 5; ++i) {
            m_scoreTexts[i].setFont(m_font);
            m_scoreTexts[i].setString(std::to_string(i + 1) + ".  " +
                                       std::to_string(scores[i]));
            m_scoreTexts[i].setCharacterSize(24);
            m_scoreTexts[i].setFillColor(sf::Color::White);
            auto bb = m_scoreTexts[i].getLocalBounds();
            m_scoreTexts[i].setPosition(w * 0.5f - bb.width * 0.5f, 210.f + i * 36.f);
        }

        m_playText.setFont(m_font);
        m_playText.setString("PLAY");
        m_playText.setCharacterSize(36);
        m_playText.setFillColor(sf::Color::Black);
        auto pb = m_playText.getLocalBounds();
        m_playText.setPosition(w * 0.5f - pb.width * 0.5f, h - 132.f);
    }

    m_playButton.setSize({160.f, 60.f});
    m_playButton.setFillColor(sf::Color{50, 200, 50});
    m_playButton.setPosition(w * 0.5f - 80.f, h - 140.f);
    m_playButtonBounds = m_playButton.getGlobalBounds();
}

void MenuState::startGame() {
    m_manager.replace(std::make_unique<GameState>(m_manager, m_window, m_score));
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        float mx = static_cast<float>(event.mouseMove.x);
        float my = static_cast<float>(event.mouseMove.y);
        m_playHovered = m_playButtonBounds.contains(mx, my);
        m_playButton.setFillColor(m_playHovered ? sf::Color{100, 255, 100}
                                                 : sf::Color{50, 200, 50});
    }
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        float mx = static_cast<float>(event.mouseButton.x);
        float my = static_cast<float>(event.mouseButton.y);
        auto b = m_playButtonBounds;
        std::cout << "click=(" << mx << "," << my << ") button=["
                  << b.left << "-" << b.left+b.width << ", "
                  << b.top  << "-" << b.top+b.height << "]\n" << std::flush;
        if (m_playButtonBounds.contains(mx, my)) startGame();
    }
    if (event.type == sf::Event::KeyPressed &&
        (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)) {
        startGame();
    }
}

void MenuState::update(float /*dt*/) {}

void MenuState::render() {
    m_window->clear(sf::Color{20, 20, 40});
    m_window->draw(m_playButton);
    if (m_fontLoaded) {
        m_window->draw(m_titleText);
        m_window->draw(m_scoreTitle);
        for (auto& t : m_scoreTexts) m_window->draw(t);
        m_window->draw(m_playText);
    }
}

}  // namespace representation
