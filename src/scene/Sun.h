#pragma once

#include <glm/glm.hpp>

namespace Caliber {

// One world-space sun drives both the sky disc and the PBR directional light.
struct Sun {
    bool enabled = true;
    float azimuthDegrees = 25.0f;
    float elevationDegrees = 14.0f;
    float angularRadiusDegrees = 0.53f;
    glm::vec3 color{1.0f, 0.95f, 0.85f};
    float intensity = 3.0f;

    // Direction FROM the surface TO the sun, not the direction light travels.
    // Azimuth zero faces -Z; positive azimuth turns toward +X.
    [[nodiscard]] glm::vec3 getDirection() const {
        const float azimuth = glm::radians(azimuthDegrees);
        const float elevation = glm::radians(elevationDegrees);
        return glm::vec3(
            glm::sin(azimuth) * glm::cos(elevation),
            glm::sin(elevation),
            -glm::cos(azimuth) * glm::cos(elevation)
        );
    }

    [[nodiscard]] glm::vec3 getRadiance() const {
        const float daylight = glm::smoothstep(0.0f, 0.08f, getDirection().y);
        return enabled ? color * glm::max(intensity, 0.0f) * daylight
                       : glm::vec3(0.0f);
    }
};

} // namespace Caliber
