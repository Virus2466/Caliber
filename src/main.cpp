
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

#include "renderer/Shader.h"
#include "renderer/Buffer.h"
#include "scene/Camera.h"
#include "core/UI.h"
#include <imgui.h>
#include "renderer/Texture.h"
#include "renderer/Model.h"




// ------------------- MOUSE STATE ------------------ 
static float s_lastX = 640.0f;
static float s_lastY = 360.0f;
static bool s_firstMouse = true;
static bool s_cursorCaptured = true; 

Caliber::Camera camera(glm::vec3(0.0f , 0.0f , 10.0f));

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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Caliber(Alpha 1.0)", nullptr, nullptr);
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
    // glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
    glDisable(GL_CULL_FACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallBack);
    glfwSetScrollCallback(window, scrollCallBack);

    Caliber::UI::init(window);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Caliber Running...\n";
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";

    { 
        

        

        // ------------------- SHADER ------------------ 
        Caliber::Shader shader("shaders/pbr.vert" , "shaders/pbr.frag");


        // Load Model
        auto gunModel = Caliber::Model::load(
            std::filesystem::current_path() / "assets" / "models" / "gun" / "scene.gltf"
        );
        if(!gunModel){
            std::cerr << "Failed to Load gun model\n";
            return -1;
        }

        std::cout << "[Debug] Camera pos: " 
          << camera.getPosition().x << ", "
          << camera.getPosition().y << ", "
          << camera.getPosition().z << "\n";
      
        
        // State Variables
        float lastFrame = 0.0f;
        bool s_tabWasPressed = false;
        glm::vec3 modelPosition(0.0f,0.0f,0.0f);
        static glm::vec3 albedo(0.8f,0.8f,0.8f);
        static float metallic = 0.9f;
        static float roughness = 0.2f;
        static float ao = 1.0f;

        // Light Positions and colors
        glm::vec3 lightPositions[] ={
            glm::vec3(2.0f,2.0f,2.0f),
            glm::vec3(-2.0f,2.0f,2.0f),
        };

        glm::vec3 lightColors[] = {
            glm::vec3(150.0f,150.0f,150.0f),
            glm::vec3(150.0f,150.0f,150.0f),
        };


        


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

                ImGui::Text("Transform");
                ImGui::SliderFloat3("Position", &modelPosition.x, -50.0f, 50.0f);
                ImGui::Separator();

                ImGui::Text("PBR MATERIAL");
                ImGui::ColorEdit3("Albedo", &albedo.x);
                ImGui::SliderFloat("Metallic",  &metallic,0.0f, 1.0f);
                ImGui::SliderFloat("Roughness",      &roughness,  0.0f, 1.0f);
                ImGui::SliderFloat("AO",             &ao,         0.0f, 1.0f);


                ImGui::Separator();
                ImGui::Text("Light 0");
                ImGui::SliderFloat3("Position##L0" , &lightPositions[0].x , -5.0f , 5.0f);
                ImGui::ColorEdit3("Color##L0" , &lightColors[0].x);

                ImGui::Text("Light 1");
                ImGui::SliderFloat3("Position##L1" , &lightPositions[1].x , -5.0f , 5.0f);
                ImGui::ColorEdit3("ColorEdit##L1" , &lightColors[1].x);



            ImGui::End();

            // -------------- RENDERING -----------------
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, modelPosition);
            // model = glm::rotate(model,
            //                     static_cast<float>(glfwGetTime()),
            //                     glm::vec3(0.5f, 1.0f, 0.0f));

            // model = glm::scale(model, glm::vec3(1.0f));  
            glm::mat4 view       = camera.getViewMatrix();
            glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

            shader.bind();
            // Matrices & View
            // PBR
            shader.setVec3("u_albedo" , albedo);
            shader.setFloat("u_metallic" , metallic);
            shader.setFloat("u_roughness",roughness);
            shader.setFloat("u_ao" ,ao);
            shader.setInt("u_lightCount",2);
            shader.setMat4("u_model",      model);
            shader.setMat4("u_view",       view);
            shader.setMat4("u_projection", projection);
            shader.setVec3("u_viewPos", camera.getPosition());

                    
            // upload light arrays
            for (int i = 0; i < 2; i++) {
                std::string pos   = "u_lightPositions[" + std::to_string(i) + "]";
                std::string color = "u_lightColors["    + std::to_string(i) + "]";
                shader.setVec3(pos,   lightPositions[i]);
                shader.setVec3(color, lightColors[i]);
            }


            

            // Draw Geometry
            //glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, 0);
            gunModel->draw(shader);


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

