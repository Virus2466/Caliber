/**
 * Represents a single drawable mesh with its GPU buffers.
 * A Model contains multiple Meshes (e.g gun body, slide, magazine)
 *
 * Usage:
 *  Caliber::Mesh mesh(vertices, indices);
 *  mesh.draw(shader);
 */

#pragma once

#include<vector>
#include<cstdint>

#include "renderer/Vertex.h"
#include "renderer/Buffer.h"
#include "renderer/Shader.h"


namespace Caliber{

class Mesh{
public:
    // constructor 
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void draw(Shader& shader) const;

    [[nodiscard]] const std::vector<Vertex>& getVertices() const { return m_vertices;}
    [[nodiscard]] const std::vector<uint32_t>& getIndices() const {return m_indices; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    VertexArray m_vao;
    VertexBuffer m_vbo;
    IndexBuffer m_ibo;

    void setupMesh();
};

} // namespace Caliber
