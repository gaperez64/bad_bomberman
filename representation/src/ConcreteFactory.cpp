#include "representation/ConcreteFactory.h"

#include <algorithm>
#include <array>
#include "representation/BombView.h"
#include "representation/CharacterView.h"
#include "representation/PowerUpView.h"
#include "representation/WallView.h"
#include "logic/BombPowerUp.h"
#include "logic/Enemy.h"
#include "logic/FirePowerUp.h"
#include "logic/Player.h"
#include "logic/SkatesPowerUp.h"

namespace representation {

static const sf::Color PLAYER_COLOR{0, 180, 255};
static const std::array<sf::Color, 3> ENEMY_COLORS{
    sf::Color{255, 80, 80},
    sf::Color{80, 255, 80},
    sf::Color{255, 200, 0}
};

ConcreteFactory::ConcreteFactory(std::shared_ptr<sf::RenderWindow> window)
    : m_window{std::move(window)} {}

std::shared_ptr<logic::Player> ConcreteFactory::createPlayer(float x, float y, float ts) {
    auto model = std::make_shared<logic::Player>(x, y, ts, ts, 0.8f);
    auto view = std::make_unique<CharacterView>(m_window, model, PLAYER_COLOR);
    model->addObserver(view.get());
    m_views.push_back(std::move(view));
    return model;
}

std::shared_ptr<logic::Enemy> ConcreteFactory::createEnemy(float x, float y, float ts, int id) {
    float speed = 0.5f + id * 0.05f;
    auto model = std::make_shared<logic::Enemy>(x, y, ts, ts, speed, id);
    sf::Color col = ENEMY_COLORS[id % 3];
    auto view = std::make_unique<CharacterView>(m_window, model, col);
    model->addObserver(view.get());
    m_views.push_back(std::move(view));
    return model;
}

std::shared_ptr<logic::Bomb> ConcreteFactory::createBomb(float x, float y, float ts, int radius,
                                                           std::weak_ptr<logic::Character> owner) {
    auto model = std::make_shared<logic::Bomb>(x, y, ts, ts, radius, std::move(owner));
    auto view = std::make_unique<BombView>(m_window, model);
    model->addObserver(view.get());
    m_views.push_back(std::move(view));
    return model;
}

std::shared_ptr<logic::Wall> ConcreteFactory::createWall(float x, float y, float tw, float th,
                                                           bool destructible) {
    auto model = std::make_shared<logic::Wall>(x, y, tw, th, destructible);
    auto view = std::make_unique<WallView>(m_window, model);
    model->addObserver(view.get());
    m_views.push_back(std::move(view));
    return model;
}

std::shared_ptr<logic::PowerUp> ConcreteFactory::createPowerUp(float x, float y, float ts,
                                                                  int type) {
    std::shared_ptr<logic::PowerUp> model;
    switch (type % 3) {
        case 0: model = std::make_shared<logic::FirePowerUp>(x, y, ts, ts);   break;
        case 1: model = std::make_shared<logic::BombPowerUp>(x, y, ts, ts);   break;
        default:model = std::make_shared<logic::SkatesPowerUp>(x, y, ts, ts); break;
    }
    auto view = std::make_unique<PowerUpView>(m_window, model);
    model->addObserver(view.get());
    m_views.push_back(std::move(view));
    return model;
}

void ConcreteFactory::drawAll(const Camera& camera) {
    for (auto& v : m_views) v->draw(camera);
}

void ConcreteFactory::clearDeadViews() {
    // Views are lightweight; keep them to allow death animations to finish.
    // They self-hide via m_visible flag.
}

}  // namespace representation
