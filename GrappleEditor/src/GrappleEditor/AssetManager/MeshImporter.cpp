#include "MeshImporter.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

namespace Grapple
{
    static Ref<Mesh> ProcessMeshNode(aiNode* node, const aiScene* scene)
    {
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* nodeMesh = scene->mMeshes[node->mMeshes[i]];

            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<uint32_t> indices;
            std::vector<glm::vec2> uvs;

            vertices.resize(nodeMesh->mNumVertices);
            normals.resize(nodeMesh->mNumVertices);
            uvs.resize(nodeMesh->mNumVertices);

            for (size_t i = 0; i < vertices.size(); i++)
            {
                vertices[i].x = nodeMesh->mVertices[i].x;
                vertices[i].y = nodeMesh->mVertices[i].y;
                vertices[i].z = nodeMesh->mVertices[i].z;
            }

            for (size_t i = 0; i < normals.size(); i++)
            {
                normals[i].x = nodeMesh->mNormals[i].x;
                normals[i].y = nodeMesh->mNormals[i].y;
                normals[i].z = nodeMesh->mNormals[i].z;
            }

            if (nodeMesh->mTextureCoords != nullptr && nodeMesh->mTextureCoords[0] != nullptr)
            {
                for (size_t i = 0; i < uvs.size(); i++)
                {
                    auto uv = nodeMesh->mTextureCoords[0][i];
                    uvs[i].x = uv.x;
                    uvs[i].y = uv.y;
                }
            }

            for (uint32_t face = 0; face < nodeMesh->mNumFaces; face++)
            {
                aiFace& f = nodeMesh->mFaces[face];

                size_t start = indices.size();
                for (uint32_t i = 0; i < f.mNumIndices; i++)
                    indices.push_back(f.mIndices[i]);

                // Swap winding order
                std::swap(indices[start], indices[start + 1]);
            }

            // TODO: import all meshes, when the submeshes are fully implemented
            // Import the first mesh for now.
            return CreateRef<Mesh>(vertices.data(), vertices.size(),
                indices.data(), indices.size(),
                normals.data(),
                uvs.data());
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            Ref<Mesh> mesh = ProcessMeshNode(node->mChildren[i], scene);
            if (mesh != nullptr)
                return mesh;
        }

        return nullptr;
    }

    static void ImportMaterials(const AssetMetadata& metadata, const aiScene* scene)
    {
        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
        std::optional<AssetHandle> defaultShader = ShaderLibrary::FindShader("Mesh");

        std::unordered_map<std::string, AssetHandle> nameToHandle;

        for (AssetHandle subAsset : metadata.SubAssets)
        {
            if (const auto* subAssetMetadata = AssetManager::GetAssetMetadata(subAsset))
                nameToHandle[subAssetMetadata->Name] = subAsset;
        }

        if (!defaultShader)
        {
            Grapple_CORE_ERROR("Failed to find 'Mesh' shader");
            return;
        }

        std::optional<uint32_t> colorProperty;
        std::optional<uint32_t> roughnessProperty;
        std::optional<uint32_t> textureProperty;

        Ref<Shader> shader = AssetManager::GetAsset<Shader>(defaultShader.value());
        if (shader != nullptr && shader->IsLoaded())
        {
            colorProperty = shader->GetPropertyIndex("u_InstanceData.Color");
            roughnessProperty = shader->GetPropertyIndex("u_InstanceData.Roughness");
            textureProperty = shader->GetPropertyIndex("u_Texture");
        }

        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            auto& material = scene->mMaterials[i];

            std::string name = material->GetName().C_Str();
            aiString texturePath;
            aiTextureMapping mapping;
            uint32_t uvIndex;
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath, &mapping, &uvIndex);

            AssetHandle baseColorTextureHandle = NULL_ASSET_HANDLE;
            if (texturePath.length > 0)
            {
                std::filesystem::path baseColorTexturePath = metadata.Path.parent_path() / texturePath.C_Str();
                if (std::filesystem::exists(baseColorTexturePath))
                {
                    std::optional<AssetHandle> handle = assetManager->FindAssetByPath(baseColorTexturePath);
                    if (handle)
                        baseColorTextureHandle = handle.value();
                    else
                        baseColorTextureHandle = assetManager->ImportAsset(baseColorTexturePath);
                }
            }

            aiColor4D color;
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            float roughness;
            material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

            Ref<Material> materialAsset = CreateRef<Material>(defaultShader.value());

            if (colorProperty)
                materialAsset->WritePropertyValue(*colorProperty, glm::vec4(color.r, color.g, color.b, color.a));
            if (roughnessProperty)
                materialAsset->WritePropertyValue(*roughnessProperty, roughness);
            if (textureProperty)
                materialAsset->WritePropertyValue(*textureProperty, baseColorTextureHandle);

            auto it = nameToHandle.find(name);
            if (it != nameToHandle.end())
                assetManager->SetLoadedAsset(it->second, materialAsset);
            else
                assetManager->ImportMemoryOnlyAsset(name, materialAsset, metadata.Handle);
        }
    }

    Ref<Mesh> MeshImporter::ImportMesh(const AssetMetadata& metadata)
    {
        std::underlying_type_t<aiPostProcessSteps> postProcessSteps = aiProcess_Triangulate;
        if (metadata.Path.extension() == ".fbx")
            postProcessSteps |= aiProcess_FlipUVs;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(metadata.Path.string(), postProcessSteps);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Grapple_CORE_ERROR("Failed to load mesh {}: {}", metadata.Path.generic_string(), importer.GetErrorString());
            return nullptr;
        }

        ImportMaterials(metadata, scene);

        return ProcessMeshNode(scene->mRootNode, scene);
    }
}
