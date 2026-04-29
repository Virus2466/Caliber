#include "renderer/Shader.h"
#include "Shader.h"
#include "glm/fwd.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include<sstream>
#include<iostream>

#include<glm/gtc/type_ptr.hpp>
#include <string_view>
#include <utility>


namespace Caliber{
    // Constructor
    Shader::Shader(const std::filesystem::path& vertPath , 
    const std::filesystem::path& fragPath){
        std::string vertSrc = readFile(vertPath);
        std::string fragSrc = readFile(fragPath);

        uint32_t vert = compileShader(GL_VERTEX_SHADER, vertSrc);
        uint32_t frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

        m_id = linkProgram(vert, frag);

        // Delete shaders after linking
        glDeleteShader(vert);
        glDeleteShader(frag);

    }

    // Move Constructor
    Shader::Shader(Shader&& other) noexcept
    : m_id(std::exchange(other.m_id , 0))
    , m_uniformCache(std::move(other.m_uniformCache)) {}


    // Move Assignment
    Shader& Shader::operator=(Shader&& other) noexcept{
        if(this != &other){
            if(m_id != 0) glDeleteProgram(m_id);
            m_id = std::exchange(other.m_id , 0);
            m_uniformCache = std::move(other.m_uniformCache);
        }
        return *this;
    }

    // Destructor
    Shader::~Shader(){
        if(m_id != 0){
            glDeleteProgram(m_id);
        }
    }

    // -----------------Uniform setters --------------
    void Shader::setInt(std::string_view name , int value) const {
        glUniform1i(getUniformLocation(name) , value);
    }

    void Shader::setFloat(std::string_view name , float value) const {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setVec3(std::string_view name, const glm::vec3& value) const {
        glUniform3fv(getUniformLocation(name) , 1 , glm::value_ptr(value));
    }

    void Shader::setVec4(std::string_view name, const glm::vec4& value) const {
        glUniform4fv(getUniformLocation(name) , 1 , glm::value_ptr(value));
    }

    void Shader::setMat4(std::string_view name, const glm::mat4& value) const {
        glUniformMatrix4fv(getUniformLocation(name) , 1 , GL_FALSE , glm::value_ptr(value));
    }

    // --------------- PRIVATE HELPERS ---------------------
    std::string Shader::readFile(const std::filesystem::path& path){
        std::ifstream file(path);

        if(!file.is_open()){
            std::cerr << "[Shader] Failed to Open : " << path << "\n";
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();

        // // temp debug 
        // std::cerr << "[SHADER] Loaded : " << path << " (" << content.size() << "bytes)\n";
        // return content;
    }


    uint32_t Shader::compileShader(GLenum type , const std::string& source){
        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id , 1, &src , nullptr);
        glCompileShader(id);

        // Checking for compile errors
        int success;
        glGetShaderiv(id,GL_COMPILE_STATUS , &success);
        if(!success){
            char log[512];
            glGetShaderInfoLog(id,512,nullptr,log);
            std::cerr << "[Shader] Compiler Error ("
                      << (type == GL_VERTEX_SHADER ? "Vertex" : "fragment")
                      << "):\n" << log << "\n";
        }
        
        return id;
    }

    uint32_t Shader::linkProgram(uint32_t vert , uint32_t frag){
        uint32_t id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id,frag);
        glLinkProgram(id);

        // check for link errors
        int success;
        glGetProgramiv(id, GL_LINK_STATUS , &success);
        if(!success){
            char log[512];
            glGetProgramInfoLog(id,512,nullptr,log);
            std::cerr << "[Shader] Link error:\n" << log << "\n";
        }

        return id;
    }



    int Shader::getUniformLocation(std::string_view name) const {
        // Check cache first
        auto it = m_uniformCache.find(std::string(name));
        if(it != m_uniformCache.end())
            return it->second;

        // not cached - ask OpenGL and store it
        int location = glGetUniformLocation(m_id , name.data());
        m_uniformCache[std::string(name)] = location;

        if(location == -1){
            std::cerr << "[Shader] Uniform not fouund :" << name << "\n";
        }

        return location;
    }


} // namespace Calber