#include "representation/BombView.h"

#include <cmath>
#include "logic/Stopwatch.h"

namespace representation {

BombView::BombView(std::shared_ptr<sf::RenderWindow> window,
                   std::shared_ptr<logic::Bomb> model)
    : EntityView{std::move(window)}, m_model{std::move(model)} {
    m_shape.setFillColor(sf::Color{20, 20, 20});
    m_shape.setOutlineColor(sf::Color{200, 50, 50});
    m_shape.setOutlineThickness(2.f);
}

void BombView::onNotify(const logic::Event& event) {
    using ET = logic::EventType;
    if (event.type == ET::BombExploded || event.type == ET::EntityDied) {
        m_visible = false;
    }
    if (event.type == ET::Tick) {
        float dt = logic::Stopwatch::instance().getDeltaTime();
        m_pulseTimer += dt * 3.f;
        m_scale = 1.f + 0.15f * std::sin(m_pulseTimer);
    }
}

void BombView::draw(const Camera& camera) {
    if (!m_visible) return;
    float px = camera.worldToPixelX(m_model->getX());
    float py = camera.worldToPixelY(m_model->getY());
    float pw = camera.worldToPixelSizeX(m_model->getWidth());
    float r = pw * 0.4f * m_scale;
    m_shape.setRadius(r);
    m_shape.setOrigin(r, r);
    m_shape.setPosition(px + pw * 0.5f, py + pw * 0.5f);
    m_window->draw(m_shape);
}

}  // namespace representation
