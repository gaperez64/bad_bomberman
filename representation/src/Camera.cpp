#include "representation/Camera.h"

namespace representation {

Camera::Camera(unsigned windowWidth, unsigned windowHeight)
    : m_width{windowWidth}, m_height{windowHeight} {}

void Camera::setWindowSize(unsigned w, unsigned h) {
    m_width = w;
    m_height = h;
}

float Camera::worldToPixelX(float wx) const {
    return (wx + 1.f) * 0.5f * static_cast<float>(m_width);
}

float Camera::worldToPixelY(float wy) const {
    return (wy + 1.f) * 0.5f * static_cast<float>(m_height);
}

float Camera::worldToPixelSizeX(float worldSize) const {
    return worldSize * 0.5f * static_cast<float>(m_width);
}

float Camera::worldToPixelSizeY(float worldSize) const {
    return worldSize * 0.5f * static_cast<float>(m_height);
}

float Camera::worldToPixelSize(float worldSize) const {
    return worldToPixelSizeX(worldSize);
}

}  // namespace representation
