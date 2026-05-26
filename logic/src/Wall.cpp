#include "logic/Wall.h"

namespace logic {

Wall::Wall(float x, float y, float w, float h, bool destructible)
    : EntityModel{x, y, w, h}, m_destructible{destructible} {}

void Wall::update(float /*dt*/) {}

}  // namespace logic
