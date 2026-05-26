#pragma once

namespace representation {

/// Projects normalized world coordinates [-1, 1] to pixel coordinates.
class Camera {
public:
    Camera(unsigned windowWidth, unsigned windowHeight);

    void setWindowSize(unsigned w, unsigned h);

    float worldToPixelX(float wx) const;
    float worldToPixelY(float wy) const;

    /// Convert a world-space width to pixels (uses x-scale)
    float worldToPixelSizeX(float worldSize) const;
    /// Convert a world-space height to pixels (uses y-scale)
    float worldToPixelSizeY(float worldSize) const;
    /// Convenience: same as worldToPixelSizeX (kept for legacy callers)
    float worldToPixelSize(float worldSize) const;

    unsigned getWindowWidth() const { return m_width; }
    unsigned getWindowHeight() const { return m_height; }

private:
    unsigned m_width{800};
    unsigned m_height{600};
};

}  // namespace representation
