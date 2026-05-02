#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

#include "renderer/Shader.h"
#include "renderer/Buffer.h"
#include "scene/Camera.h"
#include "core/UI.h"
#include <imgui.h>

// Vertices (x , y ,z)
static float vertices[] = {
    // back face
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

    // front face
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

    // left face
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

    // right face
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

    // top face
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,

    // bottom face
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
};

static uint32_t indices[] = {
    0,  1,  2,      2,  3,  0,   // back
    4,  5,  6,      6,  7,  4,   // front
    8,  9, 10,   10, 11,  8,   // left
    12, 13, 14,  14, 15, 12,   // right
    16, 17, 18,  18, 19, 16,   // top
    20, 21, 22,  22, 23, 20,   // bottom
};

// ------------------- MOUSE STATE ------------------ 
static float s_lastX = 640.0f;
static float s_lastY = 360.0f;
static bool s_firstMouse = true;
static bool s_cursorCaptured = true; 

Caliber::Camera camera(glm::vec3(0.0f , 0.0f , 3.0f));

// ------------------- CALLBACKS ------------------ 
void mouseCallBack(GLFWwindow*, double xpos , double ypos){
    if(!s_cursorCaptured) return;
    
    if(s_firstMouse){
        s_lastX = static_cast<float>(xpos);
        s_lastY = static_cast<float>(ypos);
        s_firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos) - s_lastX;
    float yOffset = s_lastY - static_cast<float>(ypos); // inverted
    s_lastX = static_cast<float>(xpos);
    s_lastY = static_cast<float>(ypos);
    
    camera.processMouse(xOffset, yOffset);
}

void scrollCallBack(GLFWwindow*, double, double yOffset){
    if(!s_cursorCaptured) return; 
    camera.processScroll(static_cast<float>(yOffset));
}

// ------------------- MAIN ------------------ 
int main(){
    // Initialize GLFW
    if(!glfwInit()){
        std::cerr << "Failed to initiate GLFW\n";
        return -1;
    }

    // OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE);

    // Create Window.
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Caliber - test", nullptr, nullptr);
    if(!window){
        std::cerr << "Failed to create Window\n";
        glfwTerminate();
        return -1; 
    }

    glfwMakeContextCurrent(window);

    // Load OpenGl functions via GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallBack);
    glfwSetScrollCallback(window, scrollCallBack);

    Caliber::UI::init(window);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Caliber Running...\n";

    { // [FIX 2]: Added scope block to ensure all OpenGL objects are destroyed before glfwTerminate()

        // ------------------- GPU Buffers ------------------ 
        Caliber::VertexArray vao;
        vao.bind();

        Caliber::VertexBuffer vbo(vertices , sizeof(vertices));
        vbo.bind();

        Caliber::IndexBuffer ibo(indices , 36);
        ibo.bind();

        vao.addAttribute(0, 3, 6 * sizeof(float), 0);
        vao.addAttribute(1, 3, 6 * sizeof(float), 3 * sizeof(float));

        // ------------------- SHADER ------------------ 
        Caliber::Shader shader("shaders/basic.vert" , "shaders/basic.frag");
        
        // State Variables
        float lastFrame = 0.0f;
        bool s_tabWasPressed = false;

        //-----------------------------------------------
        // ----------------Main Loop---------------------
        //-----------------------------------------------
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = static_cast<float>(glfwGetTime());
            float deltaTime    = currentFrame - lastFrame;
            lastFrame          = currentFrame;

            // -------------- KEYBOARD INPUT -----------------
            bool tabPressed = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
            if (tabPressed && !s_tabWasPressed) {
                s_cursorCaptured = !s_cursorCaptured;
                glfwSetInputMode(window, GLFW_CURSOR, s_cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                
                // Reset first mouse to prevent sudden camera jumps when switching back
                if(s_cursorCaptured) s_firstMouse = true; 
            }
            s_tabWasPressed = tabPressed;

            // disable camera movement when using ImGui
            if (s_cursorCaptured) {
                camera.processKeyboard(window, deltaTime);
            }
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

            // clear BOTH buffers at the TOP
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // -------------- UI SECTION -----------------
            Caliber::UI::beginFrame();
            
            ImGui::Begin("Caliber Debug");
                ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
                ImGui::Separator();

                // Light Controls 
                ImGui::Text("Point Light");
                static glm::vec3 pointLightPos(1.2f, 1.0f , 2.0f);
                ImGui::SliderFloat3("Position##Ppoint", &pointLightPos.x, -5.0f, 5.0f);

                ImGui::Separator();
                ImGui::Text("Object");
                static glm::vec3 objectColor(1.0f, 0.5f, 0.2f);
                ImGui::ColorEdit3("Color", &objectColor.x);
            ImGui::End();

            // -------------- RENDERING -----------------
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model,
                                static_cast<float>(glfwGetTime()),
                                glm::vec3(0.5f, 1.0f, 0.0f));

            glm::mat4 view       = camera.getViewMatrix();
            glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

            shader.bind();
            
            // Apply Uniforms from ImGui
            shader.setVec3("u_pointLight.position", pointLightPos);
            shader.setVec3("u_objectColor",         objectColor);

            // Directional Light (The Sun)
            shader.setVec3("u_dirLight.direction", glm::vec3(-0.2f , -1.0f, 0.3f));
            shader.setVec3("u_dirLight.ambient", glm::vec3(0.05f, 0.05f , 0.05f));
            shader.setVec3("u_dirLight.diffuse", glm::vec3(0.4f, 0.4f , 0.4f));
            shader.setVec3("u_dirLight.specular", glm::vec3(0.5f, 0.5f , 0.5f));

            // Point Light Attributes
            // [FIX 3]: Removed the hardcoded position overwrite that was right here
            shader.setVec3("u_pointLight.ambient" , glm::vec3(0.05f, 0.05f , 0.05f));
            shader.setVec3("u_pointLight.diffuse" , glm::vec3(0.8f, 0.8f , 0.8f));
            shader.setVec3("u_pointLight.specular" , glm::vec3(1.0f, 1.0f , 1.0f));
            shader.setFloat("u_pointLight.constant", 1.0f);
            shader.setFloat("u_pointLight.linear", 0.09f);
            shader.setFloat("u_pointLight.quadratic", 0.032f);

            // Spotlight (follows the camera)
            shader.setVec3("u_spotLight.position" , camera.getPosition());
            shader.setVec3("u_spotLight.direction", camera.getFront());
            shader.setVec3("u_spotLight.ambient", glm::vec3(0.0f,0.0f,0.0f));
            shader.setVec3("u_spotLight.diffuse", glm::vec3(1.0f,1.0f,1.0f));
            shader.setVec3("u_spotLight.specular", glm::vec3(1.0f,1.0f,1.0f));
            shader.setFloat("u_spotLight.constant", 1.0f);
            shader.setFloat("u_spotLight.linear", 0.09f);
            shader.setFloat("u_spotLight.quadratic", 0.032f);
            shader.setFloat("u_spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            shader.setFloat("u_spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            // Matrices & View
            shader.setMat4("u_model",      model);
            shader.setMat4("u_view",       view);
            shader.setMat4("u_projection", projection);
            shader.setVec3("u_viewPos", camera.getPosition());

            // Draw Geometry
            vao.bind();
            glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, 0);

            // Draw ImGui over the scene
            Caliber::UI::endFrame();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
    } 

    // Cleanup
    Caliber::UI::shutdown();
    glfwTerminate(); // Safe to terminate context now

    return 0;
}