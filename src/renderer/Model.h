/**
 * @file Model.h
 * @since 2026
 * @author Dev Patel
 *
 * Loads and manages a 3D model from a GLTF/OBJ file using Assimp.
 * A Model contains multiple Meshes — for example a gun model may have
 * separate meshes for body, slide, magazine, trigger etc.
 *
 * Usage:
 *  auto model = Caliber::Model::load("assets/models/gun/gun.gltf");
 *  if (model) model->draw(shader);
 */

#pragma once

#include<vector>
#include<string>
#include<optional>
#include<filesystem>

#include "assimp/material.h"
#include "glm/fwd.hpp"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

// forward declare assimp types so we dont need to include heavy headers here.
struct aiNode;
struct aiScene;
struct aiMesh;

namespace Caliber{


struct MeshInstance{
    Mesh mesh;
    glm::mat4 localTransform { glm::mat4(1.0f)};
    std::string name;
};

class Model{
public:
    [[nodiscard]] static std::optional<Model> load(const std::filesystem::path& path);

    void draw(Shader& shader,  const glm::mat4& parentTransform) const;

    // find mesh for animation
    [[nodiscard]] MeshInstance* findMesh(const std::string& name);
    [[nodiscard]] const MeshInstance* findMesh(const std::string& name) const;


    [[nodiscard]] size_t getMeshCount() const {return m_meshInstances.size() ;}

private:
    Model() = default;
    std::vector<MeshInstance>m_meshInstances;
    std::filesystem::path m_directory;

    void processNode(aiNode* node , const aiScene* scene , const glm::mat4& parentTransform);
    Mesh processMesh(aiMesh* mesh , const aiScene* scene);
    void loadMaterialTextures(aiMaterial* material , aiTextureType type, TextureType texType , std::vector<MeshTexture>& out);

    
};



} // namespace caliber
