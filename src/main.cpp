#include <cstdint>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "renderer/Shader.h"
#include "renderer/Buffer.h"
#include "scene/Camera.h"



// (TODO) : Fix the clutter down below and keep all the necessary variables here..







// (LATER ADD ) : Maybe we can shift all the vertices in some file to cleanup the main file ????
// Vertices (x , y ,z)
static float vertices[] = {
    // back face
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
    // front face
    -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,

    // Left face  (normal points toward -X)
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

    // Bottom face (normal points toward -Y)
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
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




// ------------------- MOUSEEE ------------------ 
static float s_lastX = 640.0f;
static float s_lastY = 360.0f;
static bool s_firstMouse = true;

Caliber::Camera camera(glm::vec3(0.0f , 0.0f , 3.0f));


// ------------------- MOUSE CALLBACK------------------ 
void mouseCallBack(GLFWwindow* , double xpos , double ypos){
    if(s_firstMouse){
        s_lastX = xpos;
        s_lastY = ypos;
        s_firstMouse = false;
    }

    float xOffset = (xpos - s_lastX);
    float yOffset = -(ypos - s_lastY) ; // inverted
    s_lastX = xpos;
    s_lastY = ypos;
    
    camera.processMouse(xOffset,  yOffset);

}

void scrollCallBack(GLFWwindow* , double , double yOffset){
    camera.processScroll(static_cast<float>(yOffset));
}








int main(){
    // Initialzing GLFW
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


    // caputring mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallBack);
    glfwSetScrollCallback(window, scrollCallBack);



    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Caliber Running...\n";

    //         ********
    // ------ GPU Buffers --------
    //         ********

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
    
    // Delta Time
    float lastFrame = 0.0f;


    // Main Loop
    while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime    = currentFrame - lastFrame;
    lastFrame          = currentFrame;

    // input
    camera.processKeyboard(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // clear BOTH buffers at the TOP
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rotate FIRST then upload
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model,
                        static_cast<float>(glfwGetTime()),
                        glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view       = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

    shader.bind();
    shader.setMat4("u_model",      model);
    shader.setMat4("u_view",       view);
    shader.setMat4("u_projection", projection);

    vao.bind();
    glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

    // Cleanup
    glfwTerminate();

    return 0;

}