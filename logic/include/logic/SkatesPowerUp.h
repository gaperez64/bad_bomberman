#pragma once

#include "PowerUp.h"

namespace logic {

class SkatesPowerUp : public PowerUp {
public:
    SkatesPowerUp(float x, float y, float w, float h);
    void apply(Character& character) override;
};

}  // namespace logic
