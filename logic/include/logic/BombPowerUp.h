#pragma once

#include "PowerUp.h"

namespace logic {

class BombPowerUp : public PowerUp {
public:
    BombPowerUp(float x, float y, float w, float h);
    void apply(Character& character) override;
};

}  // namespace logic
