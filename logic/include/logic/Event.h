#pragma once

namespace logic {

enum class EventType {
    Tick,
    EntityMoved,
    EntityDied,
    BlockDestroyed,
    PowerUpPickedUp,
    EnemyKilled,
    PlayerWon,
    PlayerLost,
    BombPlaced,
    BombExploded,
    GameOver
};

struct Event {
    EventType type{EventType::Tick};
    void* data{nullptr};
};

}  // namespace logic
