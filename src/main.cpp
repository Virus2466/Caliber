#include <cstdint>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>



#include "renderer/Shader.h"
#include "renderer/Buffer.h"

// Triangle Vertices (x , y ,z)
static float vertices[] = {
    -0.5f , -0.5f , 0.0f,
    0.5f , -0.5f , 0.0f,
    0.0f , 0.5f , 0.0f
};

static uint32_t indices[] = {
    0 , 1, 2
};


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

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Caliber Running...\n";

    //         ********
    // ------ GPU Buffers --------
    //         ********

    Caliber::VertexArray vao;
    vao.bind();
    
    Caliber::VertexBuffer vbo(vertices , sizeof(vertices));
    vbo.bind();

    Caliber::IndexBuffer ibo(indices , 3);
    ibo.bind();

    vao.addAttribute(0, 3, 3 * sizeof(float), 0);


    // Shader 
    Caliber::Shader shader("shaders/basic.vert" , "shaders/basic.frag");

    // Main Loop
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        vao.bind();
        glDrawElements(GL_TRIANGLES , ibo.getCount() ,GL_UNSIGNED_INT , 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();

    return 0;

}