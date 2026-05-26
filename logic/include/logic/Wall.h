#pragma once

#include "EntityModel.h"

namespace logic {

class Wall : public EntityModel {
public:
    Wall(float x, float y, float w, float h, bool destructible);
    ~Wall() override = default;

    void update(float dt) override;

    bool isDestructible() const { return m_destructible; }

private:
    bool m_destructible{false};
};

}  // namespace logic
