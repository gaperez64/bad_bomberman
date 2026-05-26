find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_library(bomberman-rep STATIC
    src/Camera.cpp
    src/EntityView.cpp
    src/CharacterView.cpp
    src/BombView.cpp
    src/WallView.cpp
    src/PowerUpView.cpp
    src/ExplosionEffect.cpp
    src/ConcreteFactory.cpp
    src/StateManager.cpp
    src/MenuState.cpp
    src/GameState.cpp
    src/Game.cpp
)

target_include_directories(bomberman-rep PUBLIC include)
find_package(X11 REQUIRED)
target_link_libraries(bomberman-rep PUBLIC bomberman-logic sfml-graphics sfml-window sfml-system X11)
target_compile_options(bomberman-rep PRIVATE -Wall -Wextra -Wpedantic)
