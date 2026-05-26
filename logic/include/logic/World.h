#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>
#include "AbstractFactory.h"
#include "Bomb.h"
#include "Character.h"
#include "Enemy.h"
#include "Player.h"
#include "PowerUp.h"
#include "Wall.h"

namespace logic {

/// Represents one tile in the explosion wave (used during explosion resolution).
struct BlastTile {
    int col{0};
    int row{0};
};

class World : public Subject {
public:
    static constexpr int COLS = 13;
    static constexpr int ROWS = 11;

    explicit World(AbstractFactory& factory);

    void update(float dt);

    Player* getPlayer() const { return m_player.get(); }
    const std::vector<std::shared_ptr<Enemy>>& getEnemies() const { return m_enemies; }
    const std::vector<std::shared_ptr<Wall>>& getWalls() const { return m_walls; }
    const std::vector<std::shared_ptr<Bomb>>& getBombs() const { return m_bombs; }
    const std::vector<std::shared_ptr<PowerUp>>& getPowerUps() const { return m_powerUps; }

    bool isGameOver() const { return m_gameOver; }
    bool playerWon() const { return m_playerWon; }

    /// Request player to place a bomb at current position.
    void playerPlaceBomb();

    /// Convert world position to grid cell
    int toCol(float x) const;
    int toRow(float y) const;

    /// Convert grid cell centre to world coordinates
    float toCentreX(int col) const;
    float toCentreY(int row) const;

    float getTileSize() const { return m_tileSize; }

    /// Check if a grid cell is walkable (no solid entity)
    bool isCellWalkable(int col, int row) const;
    bool isCellDestructible(int col, int row) const;

    /// Blast tiles from an explosion at (col,row) with given radius
    std::vector<BlastTile> computeBlastTiles(int col, int row, int radius) const;

private:
    void generateArena();
    void spawnPowerUp(float x, float y);
    void processExplosion(Bomb& bomb);
    void resolveCollisions(Character& ch, float prevX, float prevY);
    void updateEnemyAI(float dt);
    void enemyPlaceBomb(Enemy& enemy);
    void checkGameOver();
    void cleanupDead();

    AbstractFactory& m_factory;

    float m_tileSize{0.f};  // width/height of one tile in world coords

    std::shared_ptr<Player> m_player;
    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<std::shared_ptr<Wall>> m_walls;
    std::vector<std::shared_ptr<Bomb>> m_bombs;
    std::vector<std::shared_ptr<PowerUp>> m_powerUps;

    // Grid for fast lookup: nullptr = empty, non-null = wall
    std::array<std::shared_ptr<Wall>, COLS * ROWS> m_grid{};

    bool m_gameOver{false};
    bool m_playerWon{false};

    // Score observers can listen to World events too
    int m_blocksDestroyed{0};
    int m_enemiesKilled{0};
};

}  // namespace logic
