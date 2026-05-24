#include "renderer/Mesh.h"
#include "Buffer.h"
#include <cstdint>
#include <glad/glad.h>



namespace Caliber{

Mesh::Mesh(std::vector<Vertex> vertices , std::vector<uint32_t> indices )
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_vbo(nullptr , 0),
      m_ibo(nullptr , 0)
{
    setupMesh();
}


void Mesh::setupMesh(){
    m_vao.bind();

    // upload vertex data
    m_vbo = VertexBuffer(
        reinterpret_cast<const float*>(m_vertices.data()),
        static_cast<uint32_t>(m_vertices.size() * sizeof(Vertex))
    );
    m_vbo.bind();

    // upload index data
    m_ibo = IndexBuffer(
        m_indices.data(),
        static_cast<uint32_t>(m_indices.size())
    );
    m_ibo.bind();

    // position — location 0
    m_vao.addAttribute(0, 3, sizeof(Vertex), offsetof(Vertex , position));
    
    // normal - location 1
    m_vao.addAttribute(1, 3, sizeof(Vertex), offsetof(Vertex , normal));

    // texCoord - location 2
    m_vao.addAttribute(2, 2, sizeof(Vertex), offsetof(Vertex , texCoord));

    m_vao.bind();

}

void Mesh::draw([[maybe_unused]] Shader& shader) const {
    m_vao.bind();
    glDrawElements(GL_TRIANGLES , static_cast<int>(m_indices.size()) , GL_UNSIGNED_INT , 0);
    m_vao.unbind();
}




} // namespace Caliber
