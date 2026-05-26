#pragma once

#include "EntityModel.h"

namespace logic {

class Character;

enum class PowerUpType { Fire, ExtraBomb, Skates };

class PowerUp : public EntityModel {
public:
    PowerUp(float x, float y, float w, float h, PowerUpType type);
    ~PowerUp() override = default;

    void update(float dt) override;

    PowerUpType getPowerUpType() const { return m_type; }

    /// Apply the power-up effect to the character. Kills the power-up entity.
    virtual void apply(Character& character) = 0;

private:
    PowerUpType m_type;
};

}  // namespace logic
