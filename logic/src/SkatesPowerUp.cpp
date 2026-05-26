#include "logic/SkatesPowerUp.h"
#include "logic/Character.h"

namespace logic {

SkatesPowerUp::SkatesPowerUp(float x, float y, float w, float h)
    : PowerUp{x, y, w, h, PowerUpType::Skates} {}

void SkatesPowerUp::apply(Character& character) {
    character.increaseSpeed(0.3f);
    kill();
    notify(Event{EventType::PowerUpPickedUp});
}

}  // namespace logic
