#include "scene/Viewmodel.h"
#include "glm/trigonometric.hpp"
#include<glm/gtc/matrix_transform.hpp>


namespace Caliber{

void Viewmodel::update(glm::vec2 mouseDelta , float deltaTime){
    // target sway is opp to mouse mov.
    m_targetSway = glm::vec2(
        -mouseDelta.x * m_swayAmount,
        -mouseDelta.y * m_swayAmount
    );

    // clamp target so gun does not sway very far.
    m_targetSway = glm::clamp(m_targetSway , glm::vec2(-0.05f ), glm::vec2(0.05f));

    // spring forward target - lerp with deltatime for frame independent
    m_currentSway = glm::mix(m_currentSway , m_targetSway,glm::clamp(m_swaySpeed * deltaTime , 0.0f , 1.0f));

    // spring back to center 
    m_currentSway = glm::mix(m_currentSway , glm::vec2(0.0f) , glm::clamp(m_returnSpeed * deltaTime , 0.0f , 1.0f));
}

glm::mat4 Viewmodel::getTransform(const glm::vec3& basePosition , const glm::vec3& baseRotation, float scale) const {

    glm::mat4 transform = glm::mat4(1.0f);
    
    // apply base position + sway offset
    glm::vec3 swayPosition = basePosition + glm::vec3(
        m_currentSway.x,
        m_currentSway.y,
        0.0f
    );

    transform = glm::translate(transform, swayPosition);

    // apple base rotation + slight tilt from sway
    transform = glm::rotate(transform , glm::radians(baseRotation.x + m_currentSway.y * 20.0f), glm::vec3(1.0f,0.0f,0.0f));

    transform = glm::rotate(transform , glm::radians(baseRotation.y + m_currentSway.x * 20.0f),glm::vec3(0.0f , 1.0f , 0.0f));

    transform = glm::rotate(transform,glm::radians(baseRotation.z),glm::vec3(0.0f,0.0f,1.0f));

    transform = glm::scale(transform , glm::vec3(scale));

    return transform;
}




} // namespace caliber