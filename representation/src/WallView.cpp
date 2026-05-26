#include "representation/WallView.h"

namespace representation {

WallView::WallView(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<logic::Wall> model)
    : EntityView{std::move(window)}, m_model{std::move(model)} {
    m_shape.setFillColor(m_model->isDestructible() ? sf::Color{139, 90, 43}
                                                    : sf::Color{80, 80, 80});
    m_shape.setOutlineColor(sf::Color{40, 40, 40});
    m_shape.setOutlineThickness(-1.f);
}

void WallView::onNotify(const logic::Event& event) {
    if (event.type == logic::EventType::EntityDied) m_visible = false;
}

void WallView::draw(const Camera& camera) {
    if (!m_visible) return;
    float px = camera.worldToPixelX(m_model->getX());
    float py = camera.worldToPixelY(m_model->getY());
    float pw = camera.worldToPixelSizeX(m_model->getWidth());
    float ph = camera.worldToPixelSizeY(m_model->getHeight());
    m_shape.setPosition(px, py);
    m_shape.setSize({pw, ph});
    m_window->draw(m_shape);
}

}  // namespace representation
