#include "renderer/SkyRenderer.h"

namespace Caliber {

SkyRenderer::SkyRenderer()
    : m_shader("shaders/sky.vert", "shaders/sky.frag") {}

void SkyRenderer::draw(const glm::mat4& view, const glm::mat4& projection,
                       const Sun& sun, float exposure) const {
    const GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    const GLboolean cullFace = glIsEnabled(GL_CULL_FACE);
    GLboolean depthWrite = GL_TRUE;
    GLint previousProgram = 0;
    GLint previousVAO = 0;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    m_shader.bind();
    m_shader.setMat4("u_inverseProjection", glm::inverse(projection));
    m_shader.setMat4("u_inverseViewRotation", glm::mat4(glm::transpose(glm::mat3(view))));
    m_shader.setVec3("u_sunDirection", sun.getDirection());
    m_shader.setVec3("u_sunRadiance", sun.getRadiance());
    m_shader.setFloat("u_sunAngularRadius", glm::radians(sun.angularRadiusDegrees));
    m_shader.setFloat("u_exposure", exposure);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(static_cast<GLuint>(previousVAO));
    glUseProgram(static_cast<GLuint>(previousProgram));
    glDepthMask(depthWrite);
    if (depthTest) glEnable(GL_DEPTH_TEST);
    if (cullFace) glEnable(GL_CULL_FACE);
}

} // namespace Caliber
