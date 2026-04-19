#include "renderer/Buffer.h"
#include <cstdint>
#include<utility>

namespace Caliber{
    
    // --- VERTEX BUFFER ---
    VertexBuffer::VertexBuffer(const float* data , uint32_t size){
        glGenBuffers(1,&m_id);
        glBindBuffer(GL_ARRAY_BUFFER , m_id);
        glBufferData(GL_ARRAY_BUFFER , size , data , GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer(){
        if(m_id != 0){
            glDeleteBuffers(1 , &m_id);
        }
    }

    // move 
    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept 
    : m_id(std::exchange(other.m_id , 0)) {}

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept{
        if(this != &other){
            if(m_id != 0) glDeleteBuffers(1,&m_id);
            m_id = std::exchange(other.m_id , 0);
        }
        return *this;
    }




    // --- INDEX BUFFER ---
    IndexBuffer::IndexBuffer(const uint32_t* data , uint32_t count) : m_count(count) {
        glGenBuffers(1,&m_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER , count * sizeof(uint32_t) , data , GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer(){
         if(m_id != 0){
            glDeleteBuffers(1 , &m_id);
        }
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept 
    : m_id(std::exchange(other.m_id, 0)),
      m_count(std::exchange(other.m_count, 0)) {}


    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept {
        if(this != &other){
            if(m_id != 0) glDeleteBuffers(1,&m_id);
            m_id = std::exchange(other.m_id , 0);
            m_count = std::exchange(other.m_count , 0);

        }
        return  *this;
    }



    // --- VERTEX ARRAY ---
    VertexArray::VertexArray() {
        glGenVertexArrays(1,&m_id);
    }

    VertexArray::~VertexArray(){
        if(m_id != 0){
            glDeleteVertexArrays(1,&m_id);
        }
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(std::exchange(other.m_id , 0)){}

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept{
        if(this != &other){
            if (m_id != 0) glDeleteVertexArrays(1,&m_id);
            m_id = std::exchange(other.m_id , 0);
        }
        return *this;
    }

    void VertexArray::addAttribute(uint32_t index , int count , uint32_t stride ,uint32_t offset){
        glad_glEnableVertexAttribArray(index);

        /**
            glVertexAttribPointer expects the offset as a const void* pointer — a C legacy thing from before modern OpenGL. But we have a plain integer offset.
            reinterpret_cast says "take these bytes and pretend they are a different type" -- wierd ass function in my opinion :_
        */
        glVertexAttribPointer(index , count , GL_FLOAT , GL_FALSE , stride , reinterpret_cast<const void*>(offset));
    }




} // namespace caliber
