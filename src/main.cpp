#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>



#include "renderer/Shader.h"

// Triangle Vertices (x , y ,z)
static float vertices[] = {
    -0.5f , -0.5f , 0.0f,
    0.5f , -0.5f , 0.0f,
    0.0f , 0.5f , 0.0f
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

    // GPU Buffers
    uint32_t VAO , VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER , VBO);
    glBufferData(GL_ARRAY_BUFFER , sizeof(vertices) , vertices , GL_STATIC_DRAW);

    // Tell openGL how to read the vertex data
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE , 3 * sizeof(float) , (void*)0);
    glEnableVertexAttribArray(0);

    // Shader 
    Caliber::Shader shader("shaders/basic.vert" , "shaders/basic.frag");

    // Main Loop
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES , 0 ,3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glfwTerminate();

    return 0;

}