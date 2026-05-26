#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "Camera.h"

namespace representation {

struct ExplosionTile {
    float worldX{0.f};
    float worldY{0.f};
    float tileW{0.f};
    float tileH{0.f};
    float lifetime{0.f};
    static constexpr float MAX_LIFETIME = 0.6f;
};

/// Manages short-lived explosion tile animations drawn on top of everything.
class ExplosionManager {
public:
    void addExplosion(const std::vector<std::pair<float, float>>& tiles, float tileW, float tileH);
    void update(float dt);
    void draw(sf::RenderWindow& window, const Camera& camera);

private:
    std::vector<ExplosionTile> m_tiles;
};

}  // namespace representation
