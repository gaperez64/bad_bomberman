#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityView.h"
#include "logic/Wall.h"

namespace representation {

class WallView : public EntityView {
public:
    WallView(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<logic::Wall> model);

    void onNotify(const logic::Event& event) override;
    void draw(const Camera& camera) override;

private:
    std::shared_ptr<logic::Wall> m_model;
    sf::RectangleShape m_shape;
    bool m_visible{true};
};

}  // namespace representation
