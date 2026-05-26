#include "representation/StateManager.h"

namespace representation {

void StateManager::push(std::unique_ptr<State> state) {
    if (m_states.empty()) {
        // Safe to push immediately when stack is empty (no active iteration)
        m_states.push(std::move(state));
    } else {
        m_pendingAction = Action::Push;
        m_pendingState = std::move(state);
    }
}

void StateManager::pop() {
    m_pendingAction = Action::Pop;
}

void StateManager::replace(std::unique_ptr<State> state) {
    m_pendingAction = Action::Replace;
    m_pendingState = std::move(state);
}

bool StateManager::empty() const {
    return m_states.empty();
}

void StateManager::handleEvent(const sf::Event& event) {
    if (!m_states.empty()) m_states.top()->handleEvent(event);
}

void StateManager::update(float dt) {
    applyPending();
    if (!m_states.empty()) m_states.top()->update(dt);
}

void StateManager::render() {
    if (!m_states.empty()) m_states.top()->render();
}

void StateManager::applyPending() {
    switch (m_pendingAction) {
        case Action::Push:
            m_states.push(std::move(m_pendingState));
            break;
        case Action::Pop:
            if (!m_states.empty()) m_states.pop();
            break;
        case Action::Replace:
            if (!m_states.empty()) m_states.pop();
            m_states.push(std::move(m_pendingState));
            break;
        default: break;
    }
    m_pendingAction = Action::None;
}

}  // namespace representation
