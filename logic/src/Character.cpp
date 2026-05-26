#include "logic/Character.h"

namespace logic {

Character::Character(float x, float y, float w, float h, float speed)
    : EntityModel{x, y, w, h}, m_speed{speed} {}

void Character::update(float dt) {
    if (!m_alive) return;

    float dx = 0.f, dy = 0.f;
    switch (m_moveDir) {
        case Direction::Up:    dy = -m_speed * dt; break;
        case Direction::Down:  dy =  m_speed * dt; break;
        case Direction::Left:  dx = -m_speed * dt; break;
        case Direction::Right: dx =  m_speed * dt; break;
        default: break;
    }

    if (dx != 0.f || dy != 0.f) {
        m_x += dx;
        m_y += dy;
        m_direction = m_moveDir;
        notify(Event{EventType::EntityMoved});
    }
}

void Character::setMoveDirection(Direction dir) {
    m_moveDir = dir;
}

void Character::increaseSpeed(float amount) {
    m_speed += amount;
}

void Character::increaseBombRadius() {
    ++m_bombRadius;
}

void Character::increaseMaxBombs() {
    ++m_maxBombs;
}

}  // namespace logic
