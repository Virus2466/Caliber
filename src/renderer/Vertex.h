/**
 * Author - Dev Patel
 *
 * Vertex struct used across all meshes in Caliber.
 * Matches the vertex layout expected by pbr.vert
 */

#pragma once

#include <glm/glm.hpp>

namespace Caliber{

struct Vertex{
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
    glm::vec2 texCoord{0.0f};
    glm::vec3 tangent{1.0f, 0.0f, 0.0f};
    glm::vec3 bitangent{0.0f, 0.0f, 1.0f};
};


} // namespace Caliber
