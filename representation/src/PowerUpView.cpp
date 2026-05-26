#include "representation/PowerUpView.h"

namespace representation {

PowerUpView::PowerUpView(std::shared_ptr<sf::RenderWindow> window,
                         std::shared_ptr<logic::PowerUp> model)
    : EntityView{std::move(window)}, m_model{std::move(model)} {
    sf::Color col;
    switch (m_model->getPowerUpType()) {
        case logic::PowerUpType::Fire:      col = sf::Color{255, 120, 0};  break;
        case logic::PowerUpType::ExtraBomb: col = sf::Color{255, 50, 50};  break;
        case logic::PowerUpType::Skates:    col = sf::Color{50, 200, 255}; break;
    }
    m_shape.setFillColor(sf::Color{50, 50, 50, 180});
    m_icon.setFillColor(col);
}

void PowerUpView::onNotify(const logic::Event& event) {
    if (event.type == logic::EventType::EntityDied ||
        event.type == logic::EventType::PowerUpPickedUp) {
        m_visible = false;
    }
}

void PowerUpView::draw(const Camera& camera) {
    if (!m_visible) return;
    float px = camera.worldToPixelX(m_model->getX());
    float py = camera.worldToPixelY(m_model->getY());
    float pw = camera.worldToPixelSizeX(m_model->getWidth());
    float ph = camera.worldToPixelSizeY(m_model->getHeight());

    m_shape.setPosition(px, py);
    m_shape.setSize({pw, ph});
    m_window->draw(m_shape);

    float r = pw * 0.3f;
    m_icon.setRadius(r);
    m_icon.setOrigin(r, r);
    m_icon.setPosition(px + pw * 0.5f, py + ph * 0.5f);
    m_window->draw(m_icon);
}

}  // namespace representation
