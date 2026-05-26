add_library(bomberman-logic STATIC
    src/Subject.cpp
    src/Stopwatch.cpp
    src/Random.cpp
    src/EntityModel.cpp
    src/Character.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Wall.cpp
    src/PowerUp.cpp
    src/FirePowerUp.cpp
    src/BombPowerUp.cpp
    src/SkatesPowerUp.cpp
    src/Bomb.cpp
    src/World.cpp
    src/Score.cpp
)

target_include_directories(bomberman-logic PUBLIC include)
target_compile_options(bomberman-logic PRIVATE -Wall -Wextra -Wpedantic)
