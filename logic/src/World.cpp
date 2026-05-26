#include "logic/World.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <tuple>
#include <utility>

#include "logic/BombPowerUp.h"
#include "logic/FirePowerUp.h"
#include "logic/Random.h"
#include "logic/SkatesPowerUp.h"
#include "logic/Stopwatch.h"

namespace logic {

// World occupies [-1, 1] in both axes.
// tileSize = 2.0 / COLS (width-based; assumes square tiles displayed proportionally)
World::World(AbstractFactory& factory) : m_factory{factory} {
    m_tileSize = 2.f / COLS;
    generateArena();
}

// --------------- coordinate helpers ---------------

int World::toCol(float x) const {
    return static_cast<int>((x + 1.f) / m_tileSize);
}

int World::toRow(float y) const {
    return static_cast<int>((y + 1.f) / (2.f / ROWS));
}

float World::toCentreX(int col) const {
    return -1.f + (col + 0.5f) * m_tileSize;
}

float World::toCentreY(int row) const {
    return -1.f + (row + 0.5f) * (2.f / ROWS);
}

bool World::isCellWalkable(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return false;
    auto& w = m_grid[row * COLS + col];
    if (w && w->isAlive()) return false;
    // check placed bombs
    for (auto& b : m_bombs) {
        if (b->isAlive() && b->isArmed() && b->getGridCol() == col && b->getGridRow() == row)
            return false;
    }
    return true;
}

bool World::isCellDestructible(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return false;
    auto& w = m_grid[row * COLS + col];
    return w && w->isAlive() && w->isDestructible();
}

// --------------- arena generation ---------------

static bool inCornerClearZone(int col, int row) {
    // Clear 2×2 areas around each corner spawn
    auto near = [](int v, int bound) { return v == bound || v == bound + 1; };
    return (near(col, 0) && near(row, 0)) || (near(col, 0) && row >= World::ROWS - 2) ||
           (col >= World::COLS - 2 && near(row, 0)) ||
           (col >= World::COLS - 2 && row >= World::ROWS - 2);
}

void World::generateArena() {
    auto& rng = Random::instance();
    float ts = m_tileSize;
    float rs = 2.f / ROWS;

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            float wx = -1.f + c * ts;
            float wy = -1.f + r * rs;

            // Indestructible pillars at even col AND even row
            if (c % 2 == 0 && r % 2 == 0) {
                auto wall = m_factory.createWall(wx, wy, ts, rs, false);
                m_walls.push_back(wall);
                m_grid[r * COLS + c] = wall;
                continue;
            }

            // Corner clear zones
            if (inCornerClearZone(c, r)) continue;

            // Destructible block with 75% chance, else air
            if (rng.getBool(0.75f)) {
                auto wall = m_factory.createWall(wx, wy, ts, rs, true);
                m_walls.push_back(wall);
                m_grid[r * COLS + c] = wall;
            }
        }
    }

    // Spawn player at top-left corner (col 0, row 0)
    float spawnTS = ts;
    m_player = m_factory.createPlayer(-1.f + 0.5f * spawnTS, -1.f + 0.5f * (2.f / ROWS), ts);

    // Spawn 3 enemies at other corners
    struct SpawnPos { int c; int r; };
    std::array<SpawnPos, 3> eSpawns = {{{COLS - 1, 0}, {0, ROWS - 1}, {COLS - 1, ROWS - 1}}};
    for (int i = 0; i < 3; ++i) {
        float ex = -1.f + (eSpawns[i].c + 0.5f) * ts;
        float ey = -1.f + (eSpawns[i].r + 0.5f) * (2.f / ROWS);
        m_enemies.push_back(m_factory.createEnemy(ex, ey, ts, i));
    }
}

// --------------- power-up spawning ---------------

void World::spawnPowerUp(float x, float y) {
    auto& rng = Random::instance();
    if (!rng.getBool(0.25f)) return;  // 25% chance
    int type = rng.getInt(0, 2);
    m_powerUps.push_back(m_factory.createPowerUp(x, y, m_tileSize, type));
}

// --------------- explosion logic ---------------

std::vector<BlastTile> World::computeBlastTiles(int col, int row, int radius) const {
    std::vector<BlastTile> tiles;
    tiles.push_back({col, row});

    const int dcs[] = {0, 0, -1, 1};
    const int drs[] = {-1, 1, 0, 0};

    for (int dir = 0; dir < 4; ++dir) {
        for (int step = 1; step <= radius; ++step) {
            int nc = col + dcs[dir] * step;
            int nr = row + drs[dir] * step;
            if (nc < 0 || nc >= COLS || nr < 0 || nr >= ROWS) break;

            tiles.push_back({nc, nr});

            // Indestructible wall stops blast
            auto& w = m_grid[nr * COLS + nc];
            if (w && w->isAlive()) {
                if (!w->isDestructible()) {
                    tiles.pop_back();  // don't include indestructible in blast
                }
                break;  // stop in this direction regardless
            }
        }
    }
    return tiles;
}

void World::processExplosion(Bomb& bomb) {
    int bc = bomb.getGridCol();
    int br = bomb.getGridRow();
    auto blastTiles = computeBlastTiles(bc, br, bomb.getRadius());

    // Notify owner that bomb has exploded (restores bomb count)
    if (auto owner = bomb.getOwner().lock()) {
        owner->onBombExploded();
    }

    for (auto& tile : blastTiles) {
        int tc = tile.col;
        int tr = tile.row;

        // Destroy destructible walls
        auto& w = m_grid[tr * COLS + tc];
        if (w && w->isAlive() && w->isDestructible()) {
            float wx = w->getX();
            float wy = w->getY();
            w->kill();
            notify(Event{EventType::BlockDestroyed});
            m_grid[tr * COLS + tc].reset();
            spawnPowerUp(wx, wy);
            continue;  // blast stops after first destructible per direction (handled above)
        }

        // Kill characters on blast tile
        float tx = toCentreX(tc) - m_tileSize * 0.5f;
        float ty = toCentreY(tr) - (2.f / ROWS) * 0.5f;
        float tw = m_tileSize;
        float th = 2.f / ROWS;

        if (m_player && m_player->isAlive() && m_player->overlaps(tx, ty, tw, th)) {
            m_player->kill();
            notify(Event{EventType::PlayerLost});
        }

        for (auto& enemy : m_enemies) {
            if (enemy->isAlive() && enemy->overlaps(tx, ty, tw, th)) {
                enemy->kill();
                notify(Event{EventType::EnemyKilled});
            }
        }

        // Chain reaction: trigger other bombs on blast tile
        for (auto& b : m_bombs) {
            if (b->isAlive() && !b->hasExploded() && b->getGridCol() == tc &&
                b->getGridRow() == tr) {
                b->triggerExplosion();
            }
        }

        // Destroy power-ups on blast tile
        for (auto& pu : m_powerUps) {
            if (pu->isAlive() && pu->overlaps(tx, ty, tw, th)) {
                pu->kill();
            }
        }
    }
}

// --------------- collision resolution ---------------

void World::resolveCollisions(Character& ch, float /*prevX*/, float /*prevY*/) {
    float cw = ch.getWidth();
    float ch_h = ch.getHeight();

    for (auto& wall : m_walls) {
        if (!wall->isAlive()) continue;
        if (!ch.overlaps(*wall)) continue;

        float ox = ch.getX(), oy = ch.getY();
        float wx = wall->getX(), wy = wall->getY();
        float ww = wall->getWidth(), wh = wall->getHeight();

        float overlapLeft  = (ox + cw) - wx;
        float overlapRight = (wx + ww) - ox;
        float overlapUp    = (oy + ch_h) - wy;
        float overlapDown  = (wy + wh) - oy;

        float minOverlap = std::min({overlapLeft, overlapRight, overlapUp, overlapDown});

        if (minOverlap == overlapLeft)       ch.setX(wx - cw);
        else if (minOverlap == overlapRight) ch.setX(wx + ww);
        else if (minOverlap == overlapUp)    ch.setY(wy - ch_h);
        else                                 ch.setY(wy + wh);
    }

    // Collide with armed bombs
    for (auto& bomb : m_bombs) {
        if (!bomb->isAlive() || !bomb->isArmed()) continue;
        if (!ch.overlaps(*bomb)) continue;

        float ox = ch.getX(), oy = ch.getY();
        float bx = bomb->getX(), by = bomb->getY();
        float bw = bomb->getWidth(), bh = bomb->getHeight();

        float overlapLeft  = (ox + cw) - bx;
        float overlapRight = (bx + bw) - ox;
        float overlapUp    = (oy + ch_h) - by;
        float overlapDown  = (by + bh) - oy;

        float minOverlap = std::min({overlapLeft, overlapRight, overlapUp, overlapDown});

        if (minOverlap == overlapLeft)       ch.setX(bx - cw);
        else if (minOverlap == overlapRight) ch.setX(bx + bw);
        else if (minOverlap == overlapUp)    ch.setY(by - ch_h);
        else                                 ch.setY(by + bh);
    }

    // Clamp to world boundaries
    ch.setX(std::max(-1.f, std::min(1.f - cw, ch.getX())));
    ch.setY(std::max(-1.f, std::min(1.f - ch_h, ch.getY())));
}

// --------------- bomb placement ---------------

void World::playerPlaceBomb() {
    if (!m_player || !m_player->isAlive()) return;
    if (!m_player->canPlaceBomb()) return;

    int col = toCol(m_player->getX() + m_player->getWidth() * 0.5f);
    int row = toRow(m_player->getY() + m_player->getHeight() * 0.5f);

    // Don't place if bomb already at this cell
    for (auto& b : m_bombs) {
        if (b->isAlive() && b->getGridCol() == col && b->getGridRow() == row) return;
    }

    float bx = toCentreX(col) - m_tileSize * 0.5f;
    float by = toCentreY(row) - (2.f / ROWS) * 0.5f;

    auto bomb = m_factory.createBomb(bx, by, m_tileSize, m_player->getBombRadius(),
                                      std::weak_ptr<Character>(m_player));
    bomb->setGridPos(col, row);
    m_player->onBombPlaced();
    m_bombs.push_back(std::move(bomb));
    notify(Event{EventType::BombPlaced});
}

void World::enemyPlaceBomb(Enemy& enemy) {
    if (!enemy.canPlaceBomb()) return;

    int col = toCol(enemy.getX() + enemy.getWidth() * 0.5f);
    int row = toRow(enemy.getY() + enemy.getHeight() * 0.5f);

    for (auto& b : m_bombs) {
        if (b->isAlive() && b->getGridCol() == col && b->getGridRow() == row) return;
    }

    float bx = toCentreX(col) - m_tileSize * 0.5f;
    float by = toCentreY(row) - (2.f / ROWS) * 0.5f;

    // Find shared_ptr to this enemy
    std::shared_ptr<Enemy> enemyPtr;
    for (auto& e : m_enemies) {
        if (e.get() == &enemy) { enemyPtr = e; break; }
    }

    auto bomb = m_factory.createBomb(bx, by, m_tileSize, enemy.getBombRadius(),
                                      std::weak_ptr<Character>(enemyPtr));
    bomb->setGridPos(col, row);
    enemy.onBombPlaced();
    m_bombs.push_back(std::move(bomb));
    notify(Event{EventType::BombPlaced});
}

// --------------- game over check ---------------

void World::checkGameOver() {
    if (!m_player || !m_player->isAlive()) {
        if (!m_gameOver) {
            m_gameOver = true;
            m_playerWon = false;
            notify(Event{EventType::PlayerLost});
            notify(Event{EventType::GameOver});
        }
        return;
    }

    bool anyEnemyAlive = false;
    for (auto& e : m_enemies) {
        if (e->isAlive()) { anyEnemyAlive = true; break; }
    }

    if (!anyEnemyAlive && !m_gameOver) {
        m_gameOver = true;
        m_playerWon = true;
        notify(Event{EventType::PlayerWon});
        notify(Event{EventType::GameOver});
    }
}

// --------------- cleanup ---------------

void World::cleanupDead() {
    auto isDeadShared = [](const auto& p) { return !p->isAlive(); };
    m_walls.erase(std::remove_if(m_walls.begin(), m_walls.end(), isDeadShared), m_walls.end());
    m_bombs.erase(std::remove_if(m_bombs.begin(), m_bombs.end(), isDeadShared), m_bombs.end());
    m_powerUps.erase(std::remove_if(m_powerUps.begin(), m_powerUps.end(), isDeadShared),
                     m_powerUps.end());
}

// --------------- main update ---------------

void World::update(float dt) {
    if (m_gameOver) return;

    // Update player
    if (m_player && m_player->isAlive()) {
        float px = m_player->getX(), py = m_player->getY();
        m_player->update(dt);
        resolveCollisions(*m_player, px, py);
    }

    // Update enemies + AI
    updateEnemyAI(dt);
    for (auto& enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        float ex = enemy->getX(), ey = enemy->getY();
        enemy->update(dt);
        resolveCollisions(*enemy, ex, ey);

        if (enemy->wantsToPlaceBomb()) {
            enemy->clearBombRequest();
            enemyPlaceBomb(*enemy);
        }
    }

    // Update bombs
    std::vector<Bomb*> toExplode;
    for (auto& bomb : m_bombs) {
        if (!bomb->isAlive()) continue;
        bomb->update(dt);
        if (bomb->hasExploded() && bomb->isAlive()) {
            toExplode.push_back(bomb.get());
        }
    }
    // Process explosions after all bombs updated (to handle chain reactions correctly)
    for (auto* bomb : toExplode) {
        if (bomb->isAlive()) processExplosion(*bomb);
    }

    // Power-up pickup
    for (auto& pu : m_powerUps) {
        if (!pu->isAlive()) continue;
        if (m_player && m_player->isAlive() && m_player->overlaps(*pu)) {
            pu->apply(*m_player);
            notify(Event{EventType::PowerUpPickedUp});
        }
        for (auto& enemy : m_enemies) {
            if (enemy->isAlive() && enemy->overlaps(*pu) && pu->isAlive()) {
                pu->apply(*enemy);
            }
        }
    }

    checkGameOver();
    cleanupDead();
}

// --------------- enemy AI ---------------

void World::updateEnemyAI(float dt) {
    for (auto& enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        enemy->updateAI(dt, *this);
    }
}

}  // namespace logic
