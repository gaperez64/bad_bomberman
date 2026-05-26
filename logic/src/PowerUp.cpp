#include "logic/PowerUp.h"

namespace logic {

PowerUp::PowerUp(float x, float y, float w, float h, PowerUpType type)
    : EntityModel{x, y, w, h}, m_type{type} {}

void PowerUp::update(float /*dt*/) {}

}  // namespace logic
