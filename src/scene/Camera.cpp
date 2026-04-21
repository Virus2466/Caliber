#include "scene/Camera.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>


namespace Caliber{

    // Constructor
    Camera::Camera(glm::vec3 position)
    : m_position(position)
    , m_front(glm::vec3(0.0f,0.0f,-1.0f))
    , m_up(glm::vec3(0.0f , 1.0f , 0.0f))
    , m_right(glm::vec3(1.0f, 0.0f , 0.0f))
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_speed(2.5f)
    , m_sensivity(0.1f)
    , m_fov(45.0f){ updateVectors();}



    void Camera::processKeyboard(GLFWwindow* window , float deltaTime){
        float velocity = m_speed * deltaTime;
        // Forward/Backward
        if(glfwGetKey(window , GLFW_KEY_W) == GLFW_PRESS){
            m_position += m_front * velocity;
        }
        if(glfwGetKey(window , GLFW_KEY_S) == GLFW_PRESS){
            m_position -= m_front * velocity;
        }
        
        // Strafe Left/Right
        if(glfwGetKey(window , GLFW_KEY_A) == GLFW_PRESS){
            m_position -= m_right * velocity;
        }
        if(glfwGetKey(window , GLFW_KEY_D) == GLFW_PRESS){
            m_position += m_right * velocity;
        }

        // vertical
        if(glfwGetKey(window , GLFW_KEY_SPACE) == GLFW_PRESS){
            m_position += m_up * velocity;
        }
        if(glfwGetKey(window , GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            m_position -= m_up  * velocity;
        }

    }



    void Camera::processMouse(float xOffset , float yOffset){
        m_yaw += xOffset * m_sensivity;
        m_pitch += yOffset * m_sensivity;
        m_pitch = glm::clamp(m_pitch , -89.0f , 89.0f);
        updateVectors();
    }

    void Camera::processScroll(float yOffset){
        m_fov -= yOffset;
        m_fov = glm::clamp(m_fov , 1.0f , 90.0f);
    }

    glm::mat4 Camera::getViewMatrix() const{
        return glm::lookAt(m_position , m_position +  m_front, m_up);
    }

    glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
    }


    void Camera::updateVectors(){
        // calculate front vector from yaw and pitch..
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_front = glm::normalize(front);


        // right and up follow from front
        m_right = glm::normalize(glm::cross(m_front , glm::vec3(0.0f , 1.0f , 0.0f)));
        m_up = glm::normalize(glm::cross(m_right , m_front));
    }




}// namespace caliber