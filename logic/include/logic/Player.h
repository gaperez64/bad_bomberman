#pragma once

#include "Character.h"

namespace logic {

class Player : public Character {
public:
    Player(float x, float y, float w, float h, float speed);
    ~Player() override = default;
};

}  // namespace logic
