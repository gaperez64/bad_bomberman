#include "representation/GameState.h"

#include <string>
#include "representation/MenuState.h"
#include "representation/StateManager.h"
#include "logic/Stopwatch.h"

namespace representation {

GameState::GameState(StateManager& manager, std::shared_ptr<sf::RenderWindow> window,
                     logic::Score& score)
    : State{manager},
      m_window{std::move(window)},
      m_score{score},
      m_camera{m_window->getSize().x, m_window->getSize().y},
      m_factory{m_window},
      m_world{m_factory} {
    // Register as observer for world events (score + explosions)
    m_world.addObserver(this);
    m_world.addObserver(&m_score);
    m_score.reset();

    m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/liberation-sans-fonts/LiberationSans-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Bold.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("assets/font.ttf");

    if (m_fontLoaded) {
        m_scoreText.setFont(m_font);
        m_scoreText.setCharacterSize(22);
        m_scoreText.setFillColor(sf::Color::Yellow);
        m_scoreText.setPosition(10.f, 5.f);

        m_gameOverText.setFont(m_font);
        m_gameOverText.setCharacterSize(50);
        m_gameOverText.setFillColor(sf::Color::Red);

        m_statusText.setFont(m_font);
        m_statusText.setCharacterSize(28);
        m_statusText.setFillColor(sf::Color::White);
    }
}

void GameState::onNotify(const logic::Event& event) {
    using ET = logic::EventType;
    if (event.type == ET::BombExploded) {
        // We don't have easy access to which bomb just exploded here;
        // explosion visuals are driven from GameState::update by polling.
    }
    if (event.type == ET::GameOver) {
        m_gameOverShown = true;
    }
    if (event.type == ET::PlayerWon || event.type == ET::PlayerLost) {
        m_score.saveScore();
    }
}

void GameState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Space: m_world.playerPlaceBomb(); break;
            case sf::Keyboard::Escape:
                m_score.saveScore();
                m_manager.replace(
                    std::make_unique<MenuState>(m_manager, m_window, m_score));
                break;
            default: break;
        }
    }
}

void GameState::update(float dt) {
    if (m_gameOverShown) {
        m_gameOverTimer += dt;
        if (m_gameOverTimer >= GAME_OVER_DISPLAY_TIME) {
            m_manager.replace(std::make_unique<MenuState>(m_manager, m_window, m_score));
        }
        return;
    }

    // Translate held keys to player direction
    logic::Direction dir = logic::Direction::None;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dir = logic::Direction::Up;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dir = logic::Direction::Down;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dir = logic::Direction::Left;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir = logic::Direction::Right;

    auto* player = m_world.getPlayer();
    if (player) player->setMoveDirection(dir);

    // Snapshot live bomb positions before update so we can visualize their explosions
    struct BombSnapshot { int col; int row; int radius; };
    std::vector<BombSnapshot> preBombs;
    for (auto& b : m_world.getBombs()) {
        if (b->isAlive() && !b->hasExploded())
            preBombs.push_back({b->getGridCol(), b->getGridRow(), b->getRadius()});
    }

    m_world.update(dt);
    m_score.update(dt);
    m_explosions.update(dt);

    // Any bomb that was alive before and is now dead/exploded → spawn explosion visual
    for (auto& snap : preBombs) {
        bool stillAlive = false;
        for (auto& b : m_world.getBombs()) {
            if (b->getGridCol() == snap.col && b->getGridRow() == snap.row && b->isAlive() &&
                !b->hasExploded()) {
                stillAlive = true;
                break;
            }
        }
        if (!stillAlive) {
            auto tiles = m_world.computeBlastTiles(snap.col, snap.row, snap.radius);
            std::vector<std::pair<float, float>> worldTiles;
            float hw = m_world.getTileSize() * 0.5f;
            float hh = (2.f / logic::World::ROWS) * 0.5f;
            for (auto& t : tiles) {
                worldTiles.push_back(
                    {m_world.toCentreX(t.col) - hw, m_world.toCentreY(t.row) - hh});
            }
            m_explosions.addExplosion(worldTiles, m_world.getTileSize(),
                                      2.f / logic::World::ROWS);
        }
    }
}

void GameState::render() {
    m_window->clear(sf::Color{60, 80, 60});

    // Draw arena floor grid
    sf::RectangleShape floor;
    floor.setFillColor(sf::Color{70, 100, 70});
    floor.setOutlineColor(sf::Color{50, 70, 50});
    floor.setOutlineThickness(-0.5f);
    float tileW = m_world.getTileSize();
    float tileH = 2.f / logic::World::ROWS;
    float pw = m_camera.worldToPixelSizeX(tileW);
    float ph = m_camera.worldToPixelSizeY(tileH);
    for (int r = 0; r < logic::World::ROWS; ++r) {
        for (int c = 0; c < logic::World::COLS; ++c) {
            float wx = -1.f + c * tileW;
            float wy = -1.f + r * tileH;
            float px = m_camera.worldToPixelX(wx);
            float py = m_camera.worldToPixelY(wy);
            floor.setPosition(px, py);
            floor.setSize({pw, ph});
            m_window->draw(floor);
        }
    }

    m_factory.drawAll(m_camera);
    m_explosions.draw(*m_window, m_camera);
    drawHUD();
}

void GameState::drawHUD() {
    if (!m_fontLoaded) return;

    m_scoreText.setString("Score: " + std::to_string(m_score.getCurrent()));
    m_window->draw(m_scoreText);

    if (m_gameOverShown) {
        std::string msg = m_world.playerWon() ? "YOU WIN!" : "GAME OVER";
        m_gameOverText.setString(msg);
        auto bounds = m_gameOverText.getLocalBounds();
        float wx = m_window->getSize().x * 0.5f - bounds.width * 0.5f;
        float wy = m_window->getSize().y * 0.5f - bounds.height * 0.5f;
        m_gameOverText.setPosition(wx, wy);
        m_window->draw(m_gameOverText);
    }
}

}  // namespace representation
