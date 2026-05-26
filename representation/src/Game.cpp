#include "representation/Game.h"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <cmath>
#include <cstdlib>
#include <thread>
#include "representation/MenuState.h"
#include "logic/Stopwatch.h"

namespace representation {

/// Read Xft.dpi from X11 resources to detect XWayland HiDPI scale factor.
static float detectHiDPIScale() {
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return 1.f;

    float scale = 1.f;
    const char* res = XResourceManagerString(dpy);
    if (res) {
        XrmInitialize();
        XrmDatabase db = XrmGetStringDatabase(res);
        if (db) {
            char* type = nullptr;
            XrmValue value{};
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) && value.addr) {
                float dpi = std::atof(value.addr);
                if (dpi > 0.f)
                    scale = std::round(dpi / 96.f);
            }
            XrmDestroyDatabase(db);
        }
    }
    XCloseDisplay(dpy);
    return scale < 1.f ? 1.f : scale;
}

Game::Game()
    : m_window{std::make_shared<sf::RenderWindow>(
          sf::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT}, "Bomberman",
          sf::Style::Default)},
      m_hiDPIScale{detectHiDPIScale()} {
    m_window->setKeyRepeatEnabled(false);
    m_window->requestFocus();
    m_stateManager.push(std::make_unique<MenuState>(m_stateManager, m_window, m_score));
}

/// Scale a mouse event's coordinates up by the HiDPI factor so they match
/// the SFML window coordinate space (XWayland halves them on HiDPI displays).
void Game::scaleMouseEvent(sf::Event& event) const {
    if (m_hiDPIScale == 1.f) return;
    auto scale = static_cast<int>(m_hiDPIScale);
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        event.mouseButton.x *= scale;
        event.mouseButton.y *= scale;
    } else if (event.type == sf::Event::MouseMoved) {
        event.mouseMove.x *= scale;
        event.mouseMove.y *= scale;
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        event.mouseWheelScroll.x *= scale;
        event.mouseWheelScroll.y *= scale;
    }
}

void Game::run() {
    using Clock = std::chrono::steady_clock;
    auto frameDuration = std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<float>{1.f / MAX_FPS});

    while (m_window->isOpen() && !m_stateManager.empty()) {
        auto frameStart = Clock::now();

        logic::Stopwatch::instance().tick();
        float dt = logic::Stopwatch::instance().getDeltaTime();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event{};
        while (m_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window->close();
                return;
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                m_window->close();
                return;
            }
            scaleMouseEvent(event);
            m_stateManager.handleEvent(event);
        }

        m_stateManager.update(dt);

        m_window->clear();
        m_stateManager.render();
        m_window->display();

        auto frameEnd = Clock::now();
        if (frameEnd - frameStart < frameDuration)
            std::this_thread::sleep_until(frameStart + frameDuration);
    }

    m_window->close();
}

}  // namespace representation
