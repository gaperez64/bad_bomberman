#include "logic/Enemy.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

#include "logic/World.h"

namespace logic {

Enemy::Enemy(float x, float y, float w, float h, float speed, int id)
    : Character{x, y, w, h, speed}, m_id{id} {}

// --------------- BFS helpers ---------------

struct BFSNode {
    int col{0};
    int row{0};
    int dist{0};
};

static std::vector<std::pair<int, int>> bfsPath(int sc, int sr, int ec, int er,
                                                  const World& world) {
    if (sc == ec && sr == er) return {};

    std::vector<std::vector<int>> dist(World::ROWS, std::vector<int>(World::COLS, -1));
    std::vector<std::vector<std::pair<int, int>>> parent(
        World::ROWS, std::vector<std::pair<int, int>>(World::COLS, {-1, -1}));

    std::queue<BFSNode> q;
    q.push({sc, sr, 0});
    dist[sr][sc] = 0;

    const int dcs[] = {0, 0, -1, 1};
    const int drs[] = {-1, 1, 0, 0};

    while (!q.empty()) {
        auto [c, r, d] = q.front();
        q.pop();
        if (c == ec && r == er) break;

        for (int i = 0; i < 4; ++i) {
            int nc = c + dcs[i], nr = r + drs[i];
            if (nc < 0 || nc >= World::COLS || nr < 0 || nr >= World::ROWS) continue;
            if (dist[nr][nc] != -1) continue;
            if (!world.isCellWalkable(nc, nr)) continue;
            dist[nr][nc] = d + 1;
            parent[nr][nc] = {c, r};
            q.push({nc, nr, d + 1});
        }
    }

    if (dist[er][ec] == -1) return {};

    std::vector<std::pair<int, int>> path;
    int c = ec, r = er;
    while (c != sc || r != sr) {
        path.push_back({c, r});
        auto [pc, pr] = parent[r][c];
        c = pc; r = pr;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// --------------- helpers ---------------

void Enemy::moveToward(float tx, float ty, float /*dt*/) {
    float dx = tx - (m_x + m_w * 0.5f);
    float dy = ty - (m_y + m_h * 0.5f);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f) {
        m_moveDir = Direction::None;
        return;
    }
    dx /= len; dy /= len;

    if (std::abs(dx) > std::abs(dy)) {
        m_moveDir = dx > 0 ? Direction::Right : Direction::Left;
    } else {
        m_moveDir = dy > 0 ? Direction::Down : Direction::Up;
    }
}

bool Enemy::isInDanger(World& world) const {
    int col = world.toCol(m_x + m_w * 0.5f);
    int row = world.toRow(m_y + m_h * 0.5f);

    for (auto& bomb : world.getBombs()) {
        if (!bomb->isAlive() || bomb->hasExploded()) continue;
        auto blast = world.computeBlastTiles(bomb->getGridCol(), bomb->getGridRow(),
                                              bomb->getRadius());
        for (auto& tile : blast) {
            if (tile.col == col && tile.row == row) return true;
        }
    }
    return false;
}

bool Enemy::flee(float dt, World& world) {
    int col = world.toCol(m_x + m_w * 0.5f);
    int row = world.toRow(m_y + m_h * 0.5f);

    // Collect all dangerous tiles
    std::vector<std::pair<int,int>> dangerous;
    for (auto& bomb : world.getBombs()) {
        if (!bomb->isAlive() || bomb->hasExploded()) continue;
        auto blast = world.computeBlastTiles(bomb->getGridCol(), bomb->getGridRow(),
                                              bomb->getRadius());
        for (auto& tile : blast) dangerous.push_back({tile.col, tile.row});
    }

    auto isDangerous = [&](int c, int r) {
        for (auto& [dc, dr] : dangerous)
            if (dc == c && dr == r) return true;
        return false;
    };

    // BFS to nearest safe tile
    struct Node { int c, r, dist; };
    std::queue<Node> q;
    std::vector<std::vector<bool>> visited(World::ROWS, std::vector<bool>(World::COLS, false));
    q.push({col, row, 0});
    visited[row][col] = true;

    const int dcs[] = {0, 0, -1, 1};
    const int drs[] = {-1, 1, 0, 0};

    while (!q.empty()) {
        auto [c, r, d] = q.front(); q.pop();
        if (!isDangerous(c, r)) {
            float tx = world.toCentreX(c);
            float ty = world.toCentreY(r);
            moveToward(tx, ty, dt);
            m_aiState = AIState::Flee;
            return true;
        }
        for (int i = 0; i < 4; ++i) {
            int nc = c + dcs[i], nr = r + drs[i];
            if (nc < 0 || nc >= World::COLS || nr < 0 || nr >= World::ROWS) continue;
            if (visited[nr][nc]) continue;
            if (!world.isCellWalkable(nc, nr)) continue;
            visited[nr][nc] = true;
            q.push({nc, nr, d + 1});
        }
    }
    return false;
}

bool Enemy::seekPowerUp(float dt, World& world) {
    if (world.getPowerUps().empty()) return false;

    int col = world.toCol(m_x + m_w * 0.5f);
    int row = world.toRow(m_y + m_h * 0.5f);

    // Find nearest reachable power-up
    std::shared_ptr<PowerUp> nearest;
    int nearestDist = std::numeric_limits<int>::max();

    for (auto& pu : world.getPowerUps()) {
        if (!pu->isAlive()) continue;
        int pc = world.toCol(pu->getX() + pu->getWidth() * 0.5f);
        int pr = world.toRow(pu->getY() + pu->getHeight() * 0.5f);
        auto path = bfsPath(col, row, pc, pr, world);
        if (!path.empty() && static_cast<int>(path.size()) < nearestDist) {
            nearestDist = static_cast<int>(path.size());
            nearest = pu;
        }
    }

    if (!nearest) return false;

    int tc = world.toCol(nearest->getX() + nearest->getWidth() * 0.5f);
    int tr = world.toRow(nearest->getY() + nearest->getHeight() * 0.5f);
    auto path = bfsPath(col, row, tc, tr, world);
    if (path.empty()) return false;

    auto [nc, nr] = path.front();
    moveToward(world.toCentreX(nc), world.toCentreY(nr), dt);
    m_aiState = AIState::SeekPowerUp;
    return true;
}

bool Enemy::breakWall(float dt, World& world) {
    int col = world.toCol(m_x + m_w * 0.5f);
    int row = world.toRow(m_y + m_h * 0.5f);

    const int dcs[] = {0, 0, -1, 1};
    const int drs[] = {-1, 1, 0, 0};

    // Check if adjacent to a destructible wall
    for (int i = 0; i < 4; ++i) {
        int nc = col + dcs[i], nr = row + drs[i];
        if (world.isCellDestructible(nc, nr)) {
            // Place bomb here and flee
            if (canPlaceBomb() && m_bombCooldown <= 0.f) {
                m_wantsBomb = true;
                m_bombCooldown = 3.f;
            }
            m_aiState = AIState::BreakWall;
            return flee(dt, world) || true;
        }
    }
    return false;
}

bool Enemy::hunt(float dt, World& world) {
    int col = world.toCol(m_x + m_w * 0.5f);
    int row = world.toRow(m_y + m_h * 0.5f);

    // Find nearest alive character (player or enemy)
    Character* target = nullptr;
    int nearestDist = std::numeric_limits<int>::max();

    auto checkTarget = [&](Character* ch) {
        if (!ch || !ch->isAlive()) return;
        int tc = world.toCol(ch->getX() + ch->getWidth() * 0.5f);
        int tr = world.toRow(ch->getY() + ch->getHeight() * 0.5f);
        auto path = bfsPath(col, row, tc, tr, world);
        if (!path.empty() && static_cast<int>(path.size()) < nearestDist) {
            nearestDist = static_cast<int>(path.size());
            target = ch;
        }
    };

    checkTarget(world.getPlayer());
    for (auto& e : world.getEnemies()) {
        if (e.get() != this) checkTarget(e.get());
    }

    if (!target) return false;

    int tc = world.toCol(target->getX() + target->getWidth() * 0.5f);
    int tr = world.toRow(target->getY() + target->getHeight() * 0.5f);
    auto path = bfsPath(col, row, tc, tr, world);
    if (path.empty()) return false;

    // Place bomb when adjacent to target
    if (nearestDist <= 1 && canPlaceBomb() && m_bombCooldown <= 0.f) {
        m_wantsBomb = true;
        m_bombCooldown = 3.f;
        flee(dt, world);
        return true;
    }

    auto [nc, nr] = path.front();
    moveToward(world.toCentreX(nc), world.toCentreY(nr), dt);
    m_aiState = AIState::Hunt;
    return true;
}

// --------------- main AI update ---------------

void Enemy::updateAI(float dt, World& world) {
    if (!m_alive) return;

    m_bombCooldown -= dt;

    // Priority 1: flee from danger
    if (isInDanger(world)) {
        if (flee(dt, world)) return;
    }

    // Priority 2: pick up power-ups
    if (seekPowerUp(dt, world)) return;

    // Priority 3: break walls
    if (breakWall(dt, world)) return;

    // Priority 4: hunt enemies/player
    if (hunt(dt, world)) return;

    m_moveDir = Direction::None;
    m_aiState = AIState::Idle;
}

}  // namespace logic
