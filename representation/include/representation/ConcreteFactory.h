#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "logic/AbstractFactory.h"
#include "EntityView.h"

namespace representation {

class ConcreteFactory : public logic::AbstractFactory {
public:
    explicit ConcreteFactory(std::shared_ptr<sf::RenderWindow> window);

    std::shared_ptr<logic::Player> createPlayer(float x, float y, float tileSize) override;
    std::shared_ptr<logic::Enemy> createEnemy(float x, float y, float tileSize, int id) override;
    std::shared_ptr<logic::Bomb> createBomb(float x, float y, float tileSize, int radius,
                                             std::weak_ptr<logic::Character> owner) override;
    std::shared_ptr<logic::Wall> createWall(float x, float y, float tileW, float tileH,
                                             bool destructible) override;
    std::shared_ptr<logic::PowerUp> createPowerUp(float x, float y, float tileSize,
                                                   int type) override;

    /// Draw all views each frame
    void drawAll(const Camera& camera);

    void clearDeadViews();

private:
    std::shared_ptr<sf::RenderWindow> m_window;
    std::vector<std::unique_ptr<EntityView>> m_views;
};

}  // namespace representation
