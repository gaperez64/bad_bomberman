#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "logic/Observer.h"
#include "Camera.h"

namespace representation {

class EntityView : public logic::Observer {
public:
    explicit EntityView(std::shared_ptr<sf::RenderWindow> window);
    ~EntityView() override = default;

    virtual void draw(const Camera& camera) = 0;

protected:
    std::shared_ptr<sf::RenderWindow> m_window;
};

}  // namespace representation
