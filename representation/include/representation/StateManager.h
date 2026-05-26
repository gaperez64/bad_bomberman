#pragma once

#include <memory>
#include <stack>
#include "State.h"

namespace representation {

class StateManager {
public:
    void push(std::unique_ptr<State> state);
    void pop();
    void replace(std::unique_ptr<State> state);

    bool empty() const;

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render();

private:
    void applyPending();

    std::stack<std::unique_ptr<State>> m_states;

    enum class Action { None, Push, Pop, Replace };
    Action m_pendingAction{Action::None};
    std::unique_ptr<State> m_pendingState;
};

}  // namespace representation
