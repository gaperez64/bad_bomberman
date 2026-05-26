#include "representation/ExplosionEffect.h"

#include <algorithm>
#include <cmath>

namespace representation {

void ExplosionManager::addExplosion(const std::vector<std::pair<float, float>>& tiles,
                                     float tileW, float tileH) {
    for (auto& [x, y] : tiles) {
        m_tiles.push_back({x, y, tileW, tileH, 0.f});
    }
}

void ExplosionManager::update(float dt) {
    for (auto& tile : m_tiles) tile.lifetime += dt;
    m_tiles.erase(std::remove_if(m_tiles.begin(), m_tiles.end(),
                                  [](const ExplosionTile& t) {
                                      return t.lifetime >= ExplosionTile::MAX_LIFETIME;
                                  }),
                  m_tiles.end());
}

void ExplosionManager::draw(sf::RenderWindow& window, const Camera& camera) {
    sf::RectangleShape shape;
    for (auto& tile : m_tiles) {
        float t = tile.lifetime / ExplosionTile::MAX_LIFETIME;
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.f - t));
        sf::Uint8 g = static_cast<sf::Uint8>(165 * (1.f - t));
        shape.setFillColor({255, g, 0, alpha});

        float px = camera.worldToPixelX(tile.worldX);
        float py = camera.worldToPixelY(tile.worldY);
        float pw = camera.worldToPixelSizeX(tile.tileW);
        float ph = camera.worldToPixelSizeY(tile.tileH);

        shape.setPosition(px, py);
        shape.setSize({pw, ph});
        window.draw(shape);
    }
}

}  // namespace representation
