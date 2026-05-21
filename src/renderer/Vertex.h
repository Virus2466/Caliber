/**
 * Author - Dev Patel
 *
 * Vertex struct used across all meshes in Caliber.
 * Matches the vertex layout expected by pbr.vert
 */

#include<glm/glm.hpp>

namespace Caliber{

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 textCoord;
};


} // namespace Caliber