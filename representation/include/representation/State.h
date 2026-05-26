#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace representation {

class StateManager;

class State {
public:
    explicit State(StateManager& manager) : m_manager{manager} {}
    virtual ~State() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

protected:
    StateManager& m_manager;
};

}  // namespace representation
