#include "logic/EntityModel.h"

namespace logic {

EntityModel::EntityModel(float x, float y, float w, float h)
    : m_x{x}, m_y{y}, m_w{w}, m_h{h} {}

void EntityModel::kill() {
    if (!m_alive) return;
    m_alive = false;
    notify(Event{EventType::EntityDied});
}

bool EntityModel::overlaps(const EntityModel& other) const {
    return overlaps(other.m_x, other.m_y, other.m_w, other.m_h);
}

bool EntityModel::overlaps(float ox, float oy, float ow, float oh) const {
    return m_x < ox + ow && m_x + m_w > ox && m_y < oy + oh && m_y + m_h > oy;
}

}  // namespace logic
