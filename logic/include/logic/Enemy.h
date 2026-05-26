#pragma once

#include "Character.h"

namespace logic {

class World;

enum class AIState { Idle, Flee, SeekPowerUp, BreakWall, Hunt };

class Enemy : public Character {
public:
    Enemy(float x, float y, float w, float h, float speed, int id);
    ~Enemy() override = default;

    int getId() const { return m_id; }
    AIState getAIState() const { return m_aiState; }

    /// Called every frame by World to update AI logic
    void updateAI(float dt, World& world);

    bool wantsToPlaceBomb() const { return m_wantsBomb; }
    void clearBombRequest() { m_wantsBomb = false; }

private:
    bool isInDanger(World& world) const;
    bool seekPowerUp(float dt, World& world);
    bool breakWall(float dt, World& world);
    bool hunt(float dt, World& world);
    bool flee(float dt, World& world);

    void moveToward(float tx, float ty, float dt);
    void snapToGrid(World& world);

    int m_id{0};
    AIState m_aiState{AIState::Idle};
    float m_aiTimer{0.f};
    float m_bombCooldown{0.f};
    bool m_wantsBomb{false};

    // BFS target
    float m_targetX{0.f};
    float m_targetY{0.f};
    bool m_hasTarget{false};
};

}  // namespace logic
