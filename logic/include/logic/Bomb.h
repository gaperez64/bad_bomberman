#pragma once

#include <memory>
#include "EntityModel.h"

namespace logic {

class Character;

class Bomb : public EntityModel {
public:
    Bomb(float x, float y, float w, float h, int radius, std::weak_ptr<Character> owner);
    ~Bomb() override = default;

    void update(float dt) override;

    int getRadius() const { return m_radius; }
    bool hasExploded() const { return m_exploded; }
    bool isArmed() const { return m_armed; }

    /// Called by World when the explosion is fully resolved
    void triggerExplosion();

    std::weak_ptr<Character> getOwner() const { return m_owner; }

    /// Grid column/row where bomb was placed (set by World)
    int getGridCol() const { return m_gridCol; }
    int getGridRow() const { return m_gridRow; }
    void setGridPos(int col, int row) { m_gridCol = col; m_gridRow = row; }

private:
    static constexpr float FUSE_TIME = 2.f;
    static constexpr float ARM_DELAY = 0.5f;  // delay before bomb becomes solid

    float m_fuseTimer{FUSE_TIME};
    float m_armTimer{ARM_DELAY};
    int m_radius{1};
    bool m_exploded{false};
    bool m_armed{false};
    int m_gridCol{0};
    int m_gridRow{0};
    std::weak_ptr<Character> m_owner;
};

}  // namespace logic
