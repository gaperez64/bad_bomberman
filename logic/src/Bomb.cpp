#include "logic/Bomb.h"

namespace logic {

Bomb::Bomb(float x, float y, float w, float h, int radius, std::weak_ptr<Character> owner)
    : EntityModel{x, y, w, h}, m_radius{radius}, m_owner{std::move(owner)} {}

void Bomb::update(float dt) {
    if (!m_alive || m_exploded) return;

    if (!m_armed) {
        m_armTimer -= dt;
        if (m_armTimer <= 0.f) m_armed = true;
    }

    m_fuseTimer -= dt;
    notify(Event{EventType::Tick});

    if (m_fuseTimer <= 0.f) {
        triggerExplosion();
    }
}

void Bomb::triggerExplosion() {
    if (m_exploded) return;
    m_exploded = true;
    notify(Event{EventType::BombExploded});
    kill();
}

}  // namespace logic
