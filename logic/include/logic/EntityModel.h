#pragma once

#include "Subject.h"

namespace logic {

/// Base class for all game entities. Position is in normalized world coords [-1, 1].
class EntityModel : public Subject {
public:
    EntityModel(float x, float y, float w, float h);
    virtual ~EntityModel() = default;

    virtual void update(float dt) = 0;

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_w; }
    float getHeight() const { return m_h; }

    bool isAlive() const { return m_alive; }
    void kill();

    void setX(float x) { m_x = x; }
    void setY(float y) { m_y = y; }
    void setPosition(float x, float y) { m_x = x; m_y = y; }

    /// AABB overlap test (no SFML)
    bool overlaps(const EntityModel& other) const;
    bool overlaps(float ox, float oy, float ow, float oh) const;

protected:
    float m_x{0.f};
    float m_y{0.f};
    float m_w{0.f};
    float m_h{0.f};
    bool m_alive{true};
};

}  // namespace logic
