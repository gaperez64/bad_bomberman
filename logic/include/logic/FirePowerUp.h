#pragma once

#include "PowerUp.h"

namespace logic {

class FirePowerUp : public PowerUp {
public:
    FirePowerUp(float x, float y, float w, float h);
    void apply(Character& character) override;
};

}  // namespace logic
