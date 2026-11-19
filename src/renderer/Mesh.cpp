#include "renderer/Mesh.h"
#include "Buffer.h"
#include <cstdint>
#include <glad/glad.h>



namespace Caliber{

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures,
           MeshMaterial material)
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_textures(std::move(textures)),
      m_material(material),
      m_vbo(reinterpret_cast<const float*>(m_vertices.data()), static_cast<uint32_t>(m_vertices.size() * sizeof(Vertex))),
      m_ibo(m_indices.data() , static_cast<uint32_t>(m_indices.size()))
{
    setupMesh();
}


void Mesh::setupMesh(){
    m_vao.bind();
    m_vbo.bind();
    m_ibo.bind();

    // position — location 0
    m_vao.addAttribute(0, 3, sizeof(Vertex), offsetof(Vertex , position));
    
    // normal - location 1
    m_vao.addAttribute(1, 3, sizeof(Vertex), offsetof(Vertex , normal));

    // texCoord - location 2
    m_vao.addAttribute(2, 2, sizeof(Vertex), offsetof(Vertex , texCoord));


    // tangent - location - 3
    m_vao.addAttribute(3, 3, sizeof(Vertex), offsetof(Vertex, tangent));

    // bitangent - location 4
    m_vao.addAttribute(4, 3, sizeof(Vertex), offsetof(Vertex, bitangent));

    m_vao.unbind();

}

void Mesh::draw(Shader& shader) const {
    shader.setVec3("u_baseColorFactor", m_material.baseColor);
    shader.setFloat("u_metallicFactor", m_material.metallic);
    shader.setFloat("u_roughnessFactor", m_material.roughness);
    shader.setFloat("u_normalScale", m_material.normalScale);
    shader.setFloat("u_occlusionStrength", m_material.occlusionStrength);
    uint32_t albedoSlot = 0;
    uint32_t normalSlot = 1;
    uint32_t metallicRoughnessSpot = 2;
    uint32_t aoSlot = 3;

    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallicRoughness = false;
    bool hasAO = false;

    for(const auto& meshTex : m_textures){
        switch(meshTex.type){
            case TextureType::Albedo:
                meshTex.texture->bind(albedoSlot);
                shader.setInt("u_albedoMap" , 0);
                shader.setBool("u_hasAlbedoMap", true);
                hasAlbedo = true;
                break;
            case TextureType::Normal:
                meshTex.texture->bind(normalSlot);
                shader.setInt("u_normalMap" , normalSlot);
                shader.setBool("u_hasNormalMap" , true);
                hasNormal = true;
                break;
            case TextureType::MetallicRoughness:
                meshTex.texture->bind(metallicRoughnessSpot);
                shader.setInt("u_metallicRoughnessMap" , metallicRoughnessSpot);
                shader.setBool("u_hasMetallicRoughnessMap" , true);
                hasMetallicRoughness = true;
                break;
            case TextureType::AO:
                meshTex.texture->bind(aoSlot);
                shader.setInt("u_aoMap" , aoSlot);
                shader.setBool("u_hasAOMap" , true);
                hasAO = true;
                break;
        }
    }

    if(!hasAlbedo) shader.setBool("u_hasAlbedoMap" , false);
    if(!hasNormal) shader.setBool("u_hasNormalMap" , false);
    if(!hasMetallicRoughness) shader.setBool("u_hasMetallicRoughnessMap" , false);
    if(!hasAO) shader.setBool("u_hasAOMap" , false);


    const bool restoreCulling = m_material.doubleSided && glIsEnabled(GL_CULL_FACE);
    if (restoreCulling) glDisable(GL_CULL_FACE);
    m_vao.bind();
    glDrawElements(GL_TRIANGLES , static_cast<int>(m_indices.size()) , GL_UNSIGNED_INT , 0);
    m_vao.unbind();
    if (restoreCulling) glEnable(GL_CULL_FACE);
}


} // namespace Caliber
