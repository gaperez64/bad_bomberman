#include "logic/BombPowerUp.h"
#include "logic/Character.h"

namespace logic {

BombPowerUp::BombPowerUp(float x, float y, float w, float h)
    : PowerUp{x, y, w, h, PowerUpType::ExtraBomb} {}

void BombPowerUp::apply(Character& character) {
    character.increaseMaxBombs();
    kill();
    notify(Event{EventType::PowerUpPickedUp});
}

}  // namespace logic
