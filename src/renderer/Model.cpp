
#include "renderer/Model.h"
#include "assimp/material.h"
#include "renderer/Mesh.h"

#include <cstdint>
#include<iostream>

#include <assimp/Importer.hpp>
#include <assimp/GltfMaterial.h>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include <glm/geometric.hpp>
#include <optional>

namespace Caliber{
    std::optional<Model> Model::load(const std::filesystem::path& path){
        std::cout << "[Model] Trying to load: " << std::filesystem::absolute(path) << "\n";

        

        // check if file exists before even trying Assimp
        if (!std::filesystem::exists(path)) {
            std::cerr << "[Model] File does not exist at: "
                    << std::filesystem::absolute(path) << "\n";
            return std::nullopt;
        }

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path.string(),
            aiProcess_Triangulate | // convert all faces to triangle.
            aiProcess_GenSmoothNormals | // generate normals
            aiProcess_CalcTangentSpace // calculate tangents.
        );

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cerr << "[Model] Assimp Error :" << importer.GetErrorString() << "\n";
            return std::nullopt;
        }


        if (!scene) {
            std::cerr << "[Model] scene is null\n";
            return std::nullopt;
        }

        if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            std::cerr << "[Model] scene is incomplete\n";
            return std::nullopt;
        }

        if (!scene->mRootNode) {
            std::cerr << "[Model] no root node\n";
            return std::nullopt;
        }
        
        std::cout << "[Model] Loaded successfully!\n";
        std::cout << "[Model] Meshes: "     << scene->mNumMeshes     << "\n";
        std::cout << "[Model] Materials: "  << scene->mNumMaterials  << "\n";
        std::cout << "[Model] Animations: " << scene->mNumAnimations << "\n";


        Model model;
        model.m_directory = path.parent_path();
        model.processNode(scene->mRootNode, scene , glm::mat4(1.0f));

        return model;
    }




    void Model::draw(Shader& shader,  const glm::mat4& parentTransform) const {
        GLint frontFace;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        const GLenum oppositeWinding = frontFace == GL_CCW ? GL_CW : GL_CCW;
        for(const auto& instance  : m_meshInstances){
            glm::mat4 finalTransform = parentTransform * instance.localTransform;
            const bool reflected = glm::determinant(glm::mat3(finalTransform)) < 0.0f;
            if (reflected) glFrontFace(oppositeWinding);
            shader.setMat4("u_model", finalTransform);
            instance.mesh.draw(shader);
            if (reflected) glFrontFace(static_cast<GLenum>(frontFace));
        }
    }


    void Model::processNode(aiNode* node , const aiScene* scene , const glm::mat4& parentTransform){
        
        auto& m = node->mTransformation;
        glm::mat4 nodeTransform = glm::transpose(glm::mat4(
        m.a1, m.a2, m.a3, m.a4,
        m.b1, m.b2, m.b3, m.b4,
        m.c1, m.c2, m.c3, m.c4,
        m.d1, m.d2, m.d3, m.d4
        ));

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        
        // process all meshes in this node
        for(uint32_t i = 0 ; i < node->mNumMeshes; i++){
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            MeshInstance instance {
                processMesh(mesh, scene),  // 1. mesh
                globalTransform,           // 2. localTransform
                node->mName.C_Str()        // 3. name
            };
            std::cout << "[Model] Mesh Node: " << node->mName.C_Str() << "\n";

            m_meshInstances.push_back(std::move(instance));
        }
        
        for(uint32_t i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene , globalTransform);
        }
    }


    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene){
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<MeshTexture> textures;
        MeshMaterial factors;

        vertices.reserve(mesh->mNumVertices);

        // Vertices --
        for(uint32_t i = 0 ; i < mesh->mNumVertices ; i++){
            Vertex vertex{};

            // Position
            vertex.position = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            };

            // normal
            if(mesh->HasNormals()){
                vertex.normal = {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z,
                };
            }
            
            // texture coordinates - mesh can have up to 8 UV sets , we only use the first one
            if(mesh->mTextureCoords[0]){
                vertex.texCoord = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            } else{
                vertex.texCoord = {0.0f , 0.0f};
            }

            if(mesh->HasTangentsAndBitangents()){
                vertex.tangent = {
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z,
                };
                vertex.bitangent = {
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z,
                };
            } else {
                vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);
            }

            vertices.push_back(vertex);
        }

        // Indices 
        for(uint32_t i = 0 ; i < mesh->mNumFaces ; i++){
            const aiFace& face = mesh->mFaces[i];
            for(uint32_t j = 0 ; j < face.mNumIndices ; j++){
                indices.push_back(face.mIndices[j]);
            }
        }


        // Textures
        if(mesh->mMaterialIndex < scene->mNumMaterials){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor4D baseColor;
            if (material->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS ||
                material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
                factors.baseColor = {baseColor.r, baseColor.g, baseColor.b};
            }
            material->Get(AI_MATKEY_METALLIC_FACTOR, factors.metallic);
            material->Get(AI_MATKEY_ROUGHNESS_FACTOR, factors.roughness);
            material->Get(AI_MATKEY_TWOSIDED, factors.doubleSided);
            material->Get(AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0), factors.normalScale);

            // albedo
            size_t prevSize = textures.size();
            loadMaterialTextures(material, aiTextureType_BASE_COLOR, TextureType::Albedo, textures);
            if (textures.size() == prevSize)
                loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Albedo, textures);

            // normal
            prevSize = textures.size();
            loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::Normal, textures);
            if (textures.size() == prevSize)
                loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::Normal, textures);

            // metallic roughness
            prevSize = textures.size();
            loadMaterialTextures(material, aiTextureType_GLTF_METALLIC_ROUGHNESS, TextureType::MetallicRoughness, textures);
            if (textures.size() == prevSize)
                loadMaterialTextures(material, aiTextureType_METALNESS, TextureType::MetallicRoughness, textures);
            if (textures.size() == prevSize)
                loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::MetallicRoughness, textures);

            // AO
            // The pinned glTF importer stores occlusionTexture in LIGHTMAP with a strength property.
            if (material->Get(AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_LIGHTMAP, 0),
                              factors.occlusionStrength) == AI_SUCCESS) {
                loadMaterialTextures(material, aiTextureType_LIGHTMAP, TextureType::AO, textures);
            } else {
                loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AO, textures);
            }
        }





        std::cout << "[Model] Mesh : " << mesh->mNumVertices << "vertices, " << indices.size() << " indices\n";

        return Mesh(std::move(vertices), std::move(indices), std::move(textures), factors);
    }

    void Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType texType, std::vector<MeshTexture>& out){
        std::cout << "[Model] Checking texture type" << type << " count: " << material->GetTextureCount(type) << '\n';
        
        for(uint32_t i = 0; i < material->GetTextureCount(type); i++){
            aiString texturePath;

            if(material->GetTexture(type, i, &texturePath) != AI_SUCCESS){
                continue;
            }

            std::filesystem::path path(texturePath.C_Str());
            if(path.is_relative()){
                path = m_directory / path;
            }

            if(!std::filesystem::exists(path)){
                std::cerr << "[Model] Texture file missing: " << std::filesystem::absolute(path) << "\n";
                continue;
            }

            const auto colorSpace = texType == TextureType::Albedo ? TextureColorSpace::SRGB : TextureColorSpace::Linear;
            auto [entry, inserted] = m_textureCache.try_emplace({path.lexically_normal(), colorSpace});
            if (inserted) {
                entry->second = std::make_shared<Texture>(path, colorSpace);
            }
            if (entry->second->isLoaded()) {
                out.push_back({entry->second, texType});
            }
        }
    }


    MeshInstance* Model::findMesh(const std::string& name) {
        for (auto& instance : m_meshInstances)
            if (instance.name == name)
                return &instance;
        return nullptr;
    }   

    const MeshInstance* Model::findMesh(const std::string& name) const {
            for (const auto& instance : m_meshInstances)
                if (instance.name == name)
                    return &instance;
            return nullptr;
    }

} // namespace caliber
