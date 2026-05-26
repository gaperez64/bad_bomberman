#include "logic/FirePowerUp.h"
#include "logic/Character.h"

namespace logic {

FirePowerUp::FirePowerUp(float x, float y, float w, float h)
    : PowerUp{x, y, w, h, PowerUpType::Fire} {}

void FirePowerUp::apply(Character& character) {
    character.increaseBombRadius();
    kill();
    notify(Event{EventType::PowerUpPickedUp});
}

}  // namespace logic
