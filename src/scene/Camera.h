/**
 * @file Camera.h
 * @since 20th April 2026
 * @author Dev Patel
 *
 * First person camera for navigating the Caliber scene.
 * Handles movement, mouse look and zoom.
 *
 * Usage:
 *  Caliber::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
 *  camera.processKeyboard(window, deltaTime);
 *  camera.processMouse(xOffset, yOffset);
 *  shader.setMat4("u_view", camera.getViewMatrix());
 *  shader.setMat4("u_projection", camera.getProjectionMatrix(aspectRatio));
 */


 #pragma once
 #include "glm/fwd.hpp"
#include<glm/glm.hpp>
 #include<GLFW/glfw3.h>

 namespace Caliber {
    class Camera{
        public:
            Camera(glm::vec3 position = glm::vec3(0.0f , 0.0f , 3.0f));

            // Calls these every frame.
            void processKeyboard(GLFWwindow* window , float deltaTime);
            void processMouse(float xOffset , float yOffset);
            void processScroll(float yOffset);

            // Passing intp the shader
            [[nodiscard]] glm::mat4 getViewMatrix() const;
            [[nodiscard]] glm::mat4 getProjectionMatrix(float aspectRatio) const;
            [[nodiscard]] glm::vec3 getPosition() const {return m_position;}



        private:
            glm::vec3 m_position;
            glm::vec3 m_front;
            glm::vec3 m_up;
            glm::vec3 m_right;
            
            float m_yaw;
            float m_pitch;
            float m_speed;
            float m_sensivity;
            float m_fov;

            void updateVectors();

    };


 } // namespace caliber