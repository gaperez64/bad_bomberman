#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityView.h"
#include "logic/Character.h"

namespace representation {

enum class AnimState { Idle, WalkDown, WalkUp, WalkLeft, WalkRight, Death, Dead };

class CharacterView : public EntityView {
public:
    CharacterView(std::shared_ptr<sf::RenderWindow> window,
                  std::shared_ptr<logic::Character> model, sf::Color color);
    ~CharacterView() override = default;

    void onNotify(const logic::Event& event) override;
    void draw(const Camera& camera) override;

protected:
    std::shared_ptr<logic::Character> m_model;

private:
    sf::RectangleShape m_body;
    sf::RectangleShape m_eye1;
    sf::RectangleShape m_eye2;
    sf::CircleShape m_hat;
    sf::Color m_color;

    AnimState m_animState{AnimState::Idle};
    float m_animTimer{0.f};
    int m_frame{0};
    bool m_visible{true};
    float m_deathTimer{0.f};
    static constexpr float DEATH_DURATION = 1.f;
};

}  // namespace representation
