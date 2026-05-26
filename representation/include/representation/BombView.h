#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityView.h"
#include "logic/Bomb.h"

namespace representation {

class BombView : public EntityView {
public:
    BombView(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<logic::Bomb> model);

    void onNotify(const logic::Event& event) override;
    void draw(const Camera& camera) override;

private:
    std::shared_ptr<logic::Bomb> m_model;
    sf::CircleShape m_shape;
    bool m_visible{true};
    float m_pulseTimer{0.f};
    float m_scale{1.f};
    bool m_growing{false};
};

}  // namespace representation
