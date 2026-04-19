#pragma once 

#include<cstdint>
// #include<span>
#include<glad/glad.h>


namespace Caliber {
    class VertexBuffer {
    public:
        VertexBuffer(const float* data , uint32_t size);
        ~VertexBuffer();

        // Deleting copy constructor.
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        // Move constructor and mOve assignment
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void bind()  const { glBindBuffer(GL_ARRAY_BUFFER , m_id);}
        void unbind() const {glBindBuffer(GL_ARRAY_BUFFER , 0);}
        [[nodiscard]] uint32_t getID() const { return  m_id;}

    private:
        uint32_t m_id{0};
    };



} // namespace caliber