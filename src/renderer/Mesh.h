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
#include <memory>

#include "renderer/Vertex.h"
#include "renderer/Buffer.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"




namespace Caliber{


enum class TextureType{
    Albedo,
    Normal,
    MetallicRoughness,
    AO
};

struct MeshTexture{
    std::shared_ptr<const Texture> texture;
    TextureType type;
};

struct MeshMaterial {
    glm::vec3 baseColor{1.0f};
    float metallic{1.0f};
    float roughness{1.0f};
    float normalScale{1.0f};
    float occlusionStrength{1.0f};
    bool doubleSided{false};
};

class Mesh{
public:
    // constructor 
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures,
         MeshMaterial material = {});

    void draw(Shader& shader) const;

    [[nodiscard]] const std::vector<Vertex>& getVertices() const { return m_vertices;}
    [[nodiscard]] const std::vector<uint32_t>& getIndices() const {return m_indices; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<MeshTexture> m_textures;
    MeshMaterial m_material;

    VertexArray m_vao;
    VertexBuffer m_vbo;
    IndexBuffer m_ibo;

    void setupMesh();
};

} // namespace Caliber
