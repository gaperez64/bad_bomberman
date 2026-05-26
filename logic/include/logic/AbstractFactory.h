#pragma once

#include <memory>

namespace logic {

class Player;
class Enemy;
class Bomb;
class Wall;
class PowerUp;
class Character;

class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;

    virtual std::shared_ptr<Player> createPlayer(float x, float y, float tileSize) = 0;
    virtual std::shared_ptr<Enemy> createEnemy(float x, float y, float tileSize, int id) = 0;
    virtual std::shared_ptr<Bomb> createBomb(float x, float y, float tileSize, int radius,
                                              std::weak_ptr<Character> owner) = 0;
    virtual std::shared_ptr<Wall> createWall(float x, float y, float tileW, float tileH,
                                              bool destructible) = 0;
    virtual std::shared_ptr<PowerUp> createPowerUp(float x, float y, float tileSize,
                                                    int type) = 0;
};

}  // namespace logic
