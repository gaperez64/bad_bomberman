#include <cassert>
#include <iostream>
#include <memory>

#include "logic/AbstractFactory.h"
#include "logic/Bomb.h"
#include "logic/BombPowerUp.h"
#include "logic/Character.h"
#include "logic/EntityModel.h"
#include "logic/FirePowerUp.h"
#include "logic/Player.h"
#include "logic/Random.h"
#include "logic/Score.h"
#include "logic/SkatesPowerUp.h"
#include "logic/Stopwatch.h"
#include "logic/Wall.h"
#include "logic/World.h"

// --------------- Minimal stub factory for tests ---------------

class TestFactory : public logic::AbstractFactory {
public:
    std::shared_ptr<logic::Player> createPlayer(float x, float y, float ts) override {
        return std::make_shared<logic::Player>(x, y, ts, ts, 0.8f);
    }
    std::shared_ptr<logic::Enemy> createEnemy(float x, float y, float ts, int id) override {
        return std::make_shared<logic::Enemy>(x, y, ts, ts, 0.5f, id);
    }
    std::shared_ptr<logic::Bomb> createBomb(float x, float y, float ts, int radius,
                                             std::weak_ptr<logic::Character> owner) override {
        return std::make_shared<logic::Bomb>(x, y, ts, ts, radius, std::move(owner));
    }
    std::shared_ptr<logic::Wall> createWall(float x, float y, float tw, float th,
                                             bool destructible) override {
        return std::make_shared<logic::Wall>(x, y, tw, th, destructible);
    }
    std::shared_ptr<logic::PowerUp> createPowerUp(float x, float y, float ts,
                                                   int type) override {
        switch (type % 3) {
            case 0: return std::make_shared<logic::FirePowerUp>(x, y, ts, ts);
            case 1: return std::make_shared<logic::BombPowerUp>(x, y, ts, ts);
            default: return std::make_shared<logic::SkatesPowerUp>(x, y, ts, ts);
        }
    }
};

// --------------- Tests ---------------

void testAABBCollision() {
    logic::Wall w1{0.f, 0.f, 0.2f, 0.2f, true};
    logic::Wall w2{0.1f, 0.1f, 0.2f, 0.2f, true};
    logic::Wall w3{0.5f, 0.5f, 0.2f, 0.2f, true};

    assert(w1.overlaps(w2) && "overlapping walls should collide");
    assert(!w1.overlaps(w3) && "non-overlapping walls should not collide");
    std::cout << "[PASS] AABB collision detection\n";
}

void testBombExplosionRadius() {
    TestFactory factory;
    logic::World world{factory};

    // Compute blast tiles from corner with radius 1
    auto tiles = world.computeBlastTiles(0, 0, 1);
    // Should include centre + up to 4 directions (blocked by walls/borders)
    assert(!tiles.empty() && "blast tiles must not be empty");
    std::cout << "[PASS] Bomb explosion radius computation (" << tiles.size()
              << " tiles)\n";
}

void testPowerUpEffects() {
    float ts = 0.15f;
    logic::Player player{0.f, 0.f, ts, ts, 0.8f};

    int initialRadius = player.getBombRadius();
    int initialBombs  = player.getMaxBombs();
    float initialSpeed = player.getSpeed();

    logic::FirePowerUp fire{0.f, 0.f, ts, ts};
    fire.apply(player);
    assert(player.getBombRadius() == initialRadius + 1 && "Fire should increase bomb radius");

    logic::BombPowerUp bomb{0.f, 0.f, ts, ts};
    bomb.apply(player);
    assert(player.getMaxBombs() == initialBombs + 1 && "BombPowerUp should increase max bombs");

    logic::SkatesPowerUp skates{0.f, 0.f, ts, ts};
    skates.apply(player);
    assert(player.getSpeed() > initialSpeed && "Skates should increase speed");

    std::cout << "[PASS] Power-up effects\n";
}

void testScoreAccumulation() {
    logic::Score score{"__test_scores__.txt"};
    score.reset();
    assert(score.getCurrent() == 0 && "Score should start at 0");

    score.onNotify(logic::Event{logic::EventType::BlockDestroyed});
    assert(score.getCurrent() == 10 && "Block destroyed should add 10");

    score.onNotify(logic::Event{logic::EventType::PowerUpPickedUp});
    assert(score.getCurrent() == 60 && "PowerUp should add 50");

    score.onNotify(logic::Event{logic::EventType::EnemyKilled});
    assert(score.getCurrent() == 260 && "Enemy killed should add 200");

    score.onNotify(logic::Event{logic::EventType::PlayerWon});
    assert(score.getCurrent() == 1260 && "Win bonus should add 1000");

    std::cout << "[PASS] Score accumulation\n";
    std::remove("__test_scores__.txt");
}

void testRandomSingleton() {
    auto& rng = logic::Random::instance();
    for (int i = 0; i < 100; ++i) {
        int v = rng.getInt(0, 10);
        assert(v >= 0 && v <= 10 && "Random int out of range");
        float f = rng.getFloat(0.f, 1.f);
        assert(f >= 0.f && f <= 1.f && "Random float out of range");
    }
    // Verify singleton identity
    assert(&logic::Random::instance() == &logic::Random::instance() && "Singleton broken");
    std::cout << "[PASS] Random singleton\n";
}

void testStopwatch() {
    auto& sw = logic::Stopwatch::instance();
    sw.tick();
    float dt = sw.getDeltaTime();
    assert(dt >= 0.f && "Delta time must be non-negative");
    float elapsed = sw.getElapsed();
    assert(elapsed >= 0.f && "Elapsed must be non-negative");
    std::cout << "[PASS] Stopwatch singleton\n";
}

void testWallDestructible() {
    logic::Wall indestructible{0.f, 0.f, 0.1f, 0.1f, false};
    logic::Wall destructible{0.f, 0.f, 0.1f, 0.1f, true};

    assert(!indestructible.isDestructible());
    assert(destructible.isDestructible());
    std::cout << "[PASS] Wall destructibility\n";
}

void testPlayerBombCounter() {
    float ts = 0.15f;
    logic::Player player{0.f, 0.f, ts, ts, 0.8f};

    assert(player.canPlaceBomb() && "Should be able to place bomb initially");
    player.onBombPlaced();
    assert(!player.canPlaceBomb() && "Should not be able to place second bomb");
    player.onBombExploded();
    assert(player.canPlaceBomb() && "Should be able to place bomb after explosion");
    std::cout << "[PASS] Player bomb counter\n";
}

int main() {
    std::cout << "=== Logic Library Tests ===\n";
    testAABBCollision();
    testBombExplosionRadius();
    testPowerUpEffects();
    testScoreAccumulation();
    testRandomSingleton();
    testStopwatch();
    testWallDestructible();
    testPlayerBombCounter();
    std::cout << "=== All tests passed ===\n";
    return 0;
}
