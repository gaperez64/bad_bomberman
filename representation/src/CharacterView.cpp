#include "representation/CharacterView.h"

#include <algorithm>
#include <cmath>
#include "logic/Stopwatch.h"

namespace representation {

CharacterView::CharacterView(std::shared_ptr<sf::RenderWindow> window,
                             std::shared_ptr<logic::Character> model, sf::Color color)
    : EntityView{std::move(window)}, m_model{std::move(model)}, m_color{color} {
    m_body.setFillColor(m_color);
    m_body.setOutlineColor(sf::Color::Black);
    m_body.setOutlineThickness(-1.5f);
    m_eye1.setFillColor(sf::Color::White);
    m_eye2.setFillColor(sf::Color::White);
    m_hat.setFillColor(sf::Color{static_cast<sf::Uint8>(m_color.r / 2),
                                  static_cast<sf::Uint8>(m_color.g / 2),
                                  static_cast<sf::Uint8>(m_color.b / 2)});
}

void CharacterView::onNotify(const logic::Event& event) {
    using ET = logic::EventType;
    float dt = logic::Stopwatch::instance().getDeltaTime();

    switch (event.type) {
        case ET::EntityMoved:
            switch (m_model->getDirection()) {
                case logic::Direction::Down:  m_animState = AnimState::WalkDown;  break;
                case logic::Direction::Up:    m_animState = AnimState::WalkUp;    break;
                case logic::Direction::Left:  m_animState = AnimState::WalkLeft;  break;
                case logic::Direction::Right: m_animState = AnimState::WalkRight; break;
                default: break;
            }
            m_animTimer += dt;
            if (m_animTimer > 0.15f) { m_frame = (m_frame + 1) % 3; m_animTimer = 0.f; }
            break;

        case ET::EntityDied:
            m_animState = AnimState::Death;
            m_deathTimer = 0.f;
            break;

        case ET::Tick:
            if (m_animState == AnimState::Death) {
                m_deathTimer += dt;
                if (m_deathTimer >= DEATH_DURATION) {
                    m_animState = AnimState::Dead;
                    m_visible = false;
                }
            }
            break;

        default: break;
    }
}

void CharacterView::draw(const Camera& camera) {
    if (!m_visible) return;

    float px = camera.worldToPixelX(m_model->getX());
    float py = camera.worldToPixelY(m_model->getY());
    float pw = camera.worldToPixelSizeX(m_model->getWidth());
    float ph = camera.worldToPixelSizeY(m_model->getHeight());

    float alpha = 255.f;
    if (m_animState == AnimState::Death)
        alpha = 255.f * (1.f - m_deathTimer / DEATH_DURATION);

    sf::Uint8 a = static_cast<sf::Uint8>(std::max(0.f, alpha));
    sf::Color bodyCol{m_color.r, m_color.g, m_color.b, a};

    float bobOffset = 0.f;
    if (m_animState != AnimState::Death && m_animState != AnimState::Dead &&
        m_animState != AnimState::Idle)
        bobOffset = (m_frame == 1) ? ph * 0.05f : 0.f;

    m_body.setFillColor(bodyCol);
    m_body.setPosition(px, py + bobOffset);
    m_body.setSize({pw, ph});
    m_window->draw(m_body);

    if (m_animState == AnimState::Death) return;

    float ew = pw * 0.18f;
    float eh = ph * 0.18f;
    m_eye1.setSize({ew, eh});
    m_eye1.setPosition(px + pw * 0.2f, py + ph * 0.25f + bobOffset);
    m_window->draw(m_eye1);

    m_eye2.setSize({ew, eh});
    m_eye2.setPosition(px + pw * 0.55f, py + ph * 0.25f + bobOffset);
    m_window->draw(m_eye2);

    float hr = pw * 0.3f;
    m_hat.setRadius(hr);
    m_hat.setOrigin(hr, hr);
    m_hat.setFillColor(sf::Color{static_cast<sf::Uint8>(m_color.r / 2),
                                  static_cast<sf::Uint8>(m_color.g / 2),
                                  static_cast<sf::Uint8>(m_color.b / 2), a});
    m_hat.setPosition(px + pw * 0.5f, py + bobOffset);
    m_window->draw(m_hat);
}

}  // namespace representation
