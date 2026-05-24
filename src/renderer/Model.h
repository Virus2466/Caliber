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

#include "renderer/Mesh.h"
#include "renderer/Shader.h"

// forward declare assimp types so we dont need to include heavy headers here.
struct aiNode;
struct aiScene;
struct aiMesh;

namespace Caliber{

class Model{
public:
    [[nodiscard]] static std::optional<Model> load(const std::filesystem::path& path);

    void draw(Shader& shader) const;

    [[nodiscard]] size_t getMeshCount() const {return m_meshes.size() ;}

private:
    Model() = default;
    std::vector<Mesh>m_meshes;
    std::filesystem::path m_directory;

    void processNode(aiNode* node , const aiScene* scene);
    Mesh processMesh(aiMesh* mesh , const aiScene* scene);

    
};



} // namespace caliber