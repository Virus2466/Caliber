/**
 * @file Model.cpp
 * @since 2026
 * @author Dev Patel
 */

#include "renderer/Model.h"
#include "assimp/material.h"
#include "renderer/Mesh.h"

#include <cstdint>
#include<iostream>

#include <assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include <optional>

namespace Caliber{
    std::optional<Model> Model::load(const std::filesystem::path& path){
        std::cout << "[Model] Trying to load: " << std::filesystem::absolute(path)
                    << "\n";

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
            aiProcess_FlipUVs | // flips UVs for OpenGL
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
        for(const auto& instance  : m_meshInstances){
            glm::mat4 finalTransform = parentTransform * instance.localTransform;
            shader.setVec4("u_model", finalTransform);
            instance.mesh.draw(shader);
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

            MeshInstance instance;
            instance.mesh           = processMesh(mesh, scene);
            instance.localTransform = globalTransform;
            instance.name           = node->mName.C_Str();
            m_meshes.push_back(processMesh(mesh,scene));
        }
        
        for(uint32_t i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene , globalTransform);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh ,[[maybe_unused]] const aiScene* scene){
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<MeshTexture> textures;

        vertices.reserve(mesh->mNumVertices);

        // Vertices --
        for(uint32_t i = 0 ; i < mesh->mNumVertices ; i++){
            Vertex vertex;

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
        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // albedo - color
            loadMaterialTextures(material , aiTextureType_BASE_COLOR , TextureType::Albedo, textures);
            if(textures.empty()){
                loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Albedo, textures);
            }

             // normal
            loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::Normal, textures);

            // metallic roughness
            loadMaterialTextures(material, aiTextureType_METALNESS, TextureType::MetallicRoughness, textures);

            // AO
            loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AO, textures);
        }





        std::cout << "[Model] Mesh : " << mesh->mNumVertices << "vertices, " << indices.size() << " indices\n";

        return Mesh(std::move(vertices) , std::move(indices) , std::move(textures));
    }

    void Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType texType, std::vector<MeshTexture>& out){
        for(uint32_t i = 0; i < material->GetTextureCount(type); i++){
            aiString texturePath;

            if(material->GetTexture(type, i, &texturePath) != AI_SUCCESS){
                continue;
            }

            std::filesystem::path path(texturePath.C_Str());
            if(path.is_relative()){
                path = m_directory / path;
            }

            out.emplace_back(path.string(), texType);
        }
    }

} // namespace caliber
