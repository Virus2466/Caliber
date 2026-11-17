#pragma once

#include "renderer/Buffer.h"
#include "renderer/Shader.h"
#include "scene/Sun.h"

namespace Caliber {

class SkyRenderer {
public:
    SkyRenderer();

    // Draw before scene geometry. Camera translation is deliberately ignored.
    void draw(const glm::mat4& view, const glm::mat4& projection,
              const Sun& sun, float exposure) const;

private:
    Shader m_shader;
    VertexArray m_vao; // Core profile requires a VAO even without a vertex buffer.
};

} // namespace Caliber
