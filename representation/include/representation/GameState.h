#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Camera.h"
#include "ConcreteFactory.h"
#include "ExplosionEffect.h"
#include "logic/World.h"
#include "logic/Score.h"

namespace representation {

class GameState : public State, public logic::Observer {
public:
    GameState(StateManager& manager, std::shared_ptr<sf::RenderWindow> window,
              logic::Score& score);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

    void onNotify(const logic::Event& event) override;

private:
    void drawHUD();

    std::shared_ptr<sf::RenderWindow> m_window;
    logic::Score& m_score;

    Camera m_camera;
    ConcreteFactory m_factory;
    logic::World m_world;

    ExplosionManager m_explosions;

    sf::Font m_font;
    bool m_fontLoaded{false};
    sf::Text m_scoreText;
    sf::Text m_gameOverText;
    sf::Text m_statusText;

    bool m_gameOverShown{false};
    float m_gameOverTimer{0.f};
    static constexpr float GAME_OVER_DISPLAY_TIME = 3.f;

    // Track last explosion positions for visual effect
    std::vector<std::pair<float,float>> m_pendingBlastTiles;
    float m_pendingBlastTileSize{0.f};
};

}  // namespace representation
