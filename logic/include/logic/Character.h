#pragma once

#include <vector>
#include "Direction.h"
#include "EntityModel.h"

namespace logic {

class Bomb;
class World;

class Character : public EntityModel {
public:
    Character(float x, float y, float w, float h, float speed);
    ~Character() override = default;

    void update(float dt) override;

    Direction getDirection() const { return m_direction; }
    float getSpeed() const { return m_speed; }
    int getBombRadius() const { return m_bombRadius; }
    int getMaxBombs() const { return m_maxBombs; }
    int getActiveBombs() const { return m_activeBombs; }

    void setMoveDirection(Direction dir);
    void increaseSpeed(float amount);
    void increaseBombRadius();
    void increaseMaxBombs();

    bool canPlaceBomb() const { return m_activeBombs < m_maxBombs; }
    void onBombPlaced() { ++m_activeBombs; }
    void onBombExploded() { if (m_activeBombs > 0) --m_activeBombs; }

protected:
    float m_speed{0.f};
    int m_bombRadius{1};
    int m_maxBombs{1};
    int m_activeBombs{0};
    Direction m_direction{Direction::None};
    Direction m_moveDir{Direction::None};
};

}  // namespace logic
