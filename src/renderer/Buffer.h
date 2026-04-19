/**
 * @since 20th April 2026
 * @author Dev Patel.
 * -- Buffer Class to reduce the vertices that send to the GPU , that is important for our task that is handling large vertices.
 * -- And it is also simplifying the main file.
 */


#pragma once 

#include<cstdint>
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

    // Index Buffer Object -- 
    class IndexBuffer{
        public:
            IndexBuffer(const uint32_t* data , uint32_t count);
            
            // Deleting Copy Constructor 
            IndexBuffer(const IndexBuffer&) = delete;
            IndexBuffer& operator=(const IndexBuffer&) = delete;

            // Move assignment and move constructor.
            IndexBuffer(IndexBuffer&& other) noexcept;
            IndexBuffer& operator=(IndexBuffer&& other) noexcept;

            void bind() const {glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , m_id);}
            void unbind() const {glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , 0);}

            [[nodiscard]] uint32_t getCount() const {return m_count;}
            [[nodiscard]] uint32_t getID() const { return  m_id;}

            // Destructor -- kek
            ~IndexBuffer();

        private:
            uint32_t m_id{0};
            uint32_t m_count {0};
        
    };

    class VertexArray{
        public:
            VertexArray();
            ~VertexArray();
            
            // No copyable
            VertexArray(const VertexArray&) = delete;
            VertexArray& operator=(const VertexArray&) = delete;

            // Move 
            VertexArray(VertexArray&& other) noexcept;
            VertexArray& operator=(VertexArray&& other) noexcept;

            void bind() const {glBindVertexArray(m_id);}
            void unbind() const {glBindVertexArray(0);}

            // we tell opengl how vertex data is designed ?????????????
            void addAttribute(uint32_t index , int count , uint32_t stride ,uint32_t offset);

            [[nodiscard]] uint32_t getID() const { return m_id;}

        private:
            uint32_t m_id{0};
    };


} // namespace caliber