#include <exception>
#include <iostream>
#include "representation/Game.h"

int main() {
    try {
        representation::Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
