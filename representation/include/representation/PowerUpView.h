#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityView.h"
#include "logic/PowerUp.h"

namespace representation {

class PowerUpView : public EntityView {
public:
    PowerUpView(std::shared_ptr<sf::RenderWindow> window,
                std::shared_ptr<logic::PowerUp> model);

    void onNotify(const logic::Event& event) override;
    void draw(const Camera& camera) override;

private:
    std::shared_ptr<logic::PowerUp> m_model;
    sf::RectangleShape m_shape;
    sf::CircleShape m_icon;
    bool m_visible{true};
};

}  // namespace representation
