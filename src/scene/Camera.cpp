#include "scene/Camera.h"
#include "GLFW/glfw3.h"



namespace Caliber{
    void Camera::processKeyboard(GLFWwindow* window , float deltaTime){
        float velocity = m_movementSpeed * deltaTime;
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




}// namespace caliber