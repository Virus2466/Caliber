#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>


namespace Caliber{

class Viewmodel{
public:
    Viewmodel() = default;

    // call every frame with mouse delta and deltatime
    void update(glm::vec2 mouseDelta , float deltaTime);

    // get final gun transform with sway applied
    [[nodiscard]] glm::mat4 getTransform(
        const glm::vec3& basePosition,
        const glm::vec3& baseRotation,
        float scale
    ) const;

    // setters for tuning for imGui
    void setSwayAmount(float amount) { m_swayAmount = amount; }
    void setSwaySpeed(float speed) { m_swaySpeed = speed; }
    void setReturnSpeed(float speed) { m_returnSpeed = speed; }

    [[nodiscard]] glm::vec2 getCurrentSway() const { return m_currentSway; }

private:
    glm::vec2 m_currentSway {0.0f};
    glm::vec2 m_targetSway {0.0f};

    float m_swayAmount {0.002f};
    float m_swaySpeed {8.0f};
    float m_returnSpeed {4.0f};


};



} // namespace caliber