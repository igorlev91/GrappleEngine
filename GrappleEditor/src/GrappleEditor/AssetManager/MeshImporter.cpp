#include "MeshImporter.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/MaterialsTable.h"
#include "Grapple/Renderer/ShaderLibrary.h"
#include "Grapple/Renderer/Renderer.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

namespace Grapple
{
    static Ref<Mesh> ProcessMeshNode(aiNode* node, const aiScene* scene, std::vector<uint32_t>& usedMaterials)
    {
        if (node->mNumMeshes > 0)
        {
            size_t verticesCount = 0;
            size_t indicesCount = 0;
            IndexBuffer::IndexFormat indexFormat = IndexBuffer::IndexFormat::UInt32;

            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* nodeMesh = scene->mMeshes[node->mMeshes[i]];
                verticesCount += nodeMesh->mNumVertices;

                for (uint32_t face = 0; face < nodeMesh->mNumFaces; face++)
                {
                    aiFace& f = nodeMesh->mFaces[face];
                    indicesCount += (size_t)f.mNumIndices;
                }
            }

            if (indicesCount < (size_t)std::numeric_limits<uint16_t>::max())
                indexFormat = IndexBuffer::IndexFormat::UInt16;

            Ref<Mesh> mesh = Mesh::Create(MeshTopology::Triangles, verticesCount, indexFormat, indicesCount);

            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* nodeMesh = scene->mMeshes[node->mMeshes[i]];
                usedMaterials.push_back(nodeMesh->mMaterialIndex);

                std::vector<glm::vec3> vertices;
                std::vector<glm::vec3> normals;
                std::vector<glm::vec3> tangents;
                std::vector<glm::vec2> uvs;

                vertices.resize(nodeMesh->mNumVertices);
                normals.resize(nodeMesh->mNumVertices);
                tangents.resize(nodeMesh->mNumVertices);

                std::memcpy(vertices.data(), nodeMesh->mVertices, sizeof(glm::vec3) * vertices.size());
                std::memcpy(normals.data(), nodeMesh->mNormals, sizeof(glm::vec3) * normals.size());
                std::memcpy(tangents.data(), nodeMesh->mTangents, sizeof(glm::vec3) * tangents.size());

                uvs.resize(nodeMesh->mNumVertices);
                if (nodeMesh->mTextureCoords != nullptr && nodeMesh->mTextureCoords[0] != nullptr)
                {
                    for (size_t i = 0; i < uvs.size(); i++)
                    {
                        auto uv = nodeMesh->mTextureCoords[0][i];
                        uvs[i].x = uv.x;
                        uvs[i].y = uv.y;
                    }
                }
                else
                {
                    for (size_t i = 0; i < uvs.size(); i++)
                        uvs[i] = glm::vec2(0.0f);
                }

                std::vector<uint16_t> indices16;
                std::vector<uint32_t> indices32;

                if (indexFormat == IndexBuffer::IndexFormat::UInt16)
                {
                    indices16.reserve(indicesCount);

                    for (uint32_t face = 0; face < nodeMesh->mNumFaces; face++)
                    {
                        aiFace& f = nodeMesh->mFaces[face];

                        size_t start = indices16.size();
                        for (uint32_t i = 0; i < f.mNumIndices; i++)
                            indices16.push_back((uint16_t)f.mIndices[i]);
                    }
                }
                else
                {
                    indices32.reserve(indicesCount);

                    for (uint32_t face = 0; face < nodeMesh->mNumFaces; face++)
                    {
                        aiFace& f = nodeMesh->mFaces[face];

                        size_t start = indices32.size();
                        for (uint32_t i = 0; i < f.mNumIndices; i++)
                            indices32.push_back((uint32_t)f.mIndices[i]);
                    }
                }

                mesh->AddSubMesh(
                    Span<glm::vec3>::FromVector(vertices),
                    indexFormat == IndexBuffer::IndexFormat::UInt16
                        ? MemorySpan::FromVector(indices16)
                        : MemorySpan::FromVector(indices32),
                    Span<glm::vec3>::FromVector(normals),
                    Span<glm::vec3>::FromVector(tangents),
                    Span<glm::vec2>::FromVector(uvs));
            }

            return mesh;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            Ref<Mesh> mesh = ProcessMeshNode(node->mChildren[i], scene, usedMaterials);
            if (mesh != nullptr)
                return mesh;
        }

        return nullptr;
    }

    static AssetHandle FindTextureByPath(std::string_view path, const AssetMetadata& metadata, const Ref<EditorAssetManager>& assetManager)
    {
        AssetHandle textureHandle = NULL_ASSET_HANDLE;
        if (path.size() > 0)
        {
            std::filesystem::path texturePath = metadata.Path.parent_path() / path;
            if (std::filesystem::exists(texturePath))
            {
                std::optional<AssetHandle> handle = assetManager->FindAssetByPath(texturePath);
                if (handle)
                    textureHandle = handle.value();
                else
                    textureHandle = assetManager->ImportAsset(texturePath);
            }
        }

        return textureHandle;
    }

    static void TrySetMaterialTexture(std::optional<uint32_t> propertyIndex, const Ref<Material>& material, AssetHandle handle, const Ref<Texture>& defaultValue)
    {
        if (propertyIndex)
        {
            auto& value = material->GetPropertyValue<TexturePropertyValue>(*propertyIndex);

            if (handle == NULL_ASSET_HANDLE)
                value.SetTexture(defaultValue);
            else
                value.SetTexture(AssetManager::GetAsset<Texture>(handle));
        }
    }

    static void ImportMaterials(const AssetMetadata& metadata, const aiScene* scene, const std::vector<uint32_t>& usedMaterials)
    {
        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
        std::optional<AssetHandle> defaultShader = ShaderLibrary::FindShader("Mesh");

        std::unordered_map<std::string, AssetHandle> nameToHandle;
        AssetHandle materialsTableHandle;

        for (AssetHandle subAsset : metadata.SubAssets)
        {
            if (const auto* subAssetMetadata = AssetManager::GetAssetMetadata(subAsset))
            {
                if (subAssetMetadata->Type == AssetType::Material)
                    nameToHandle[subAssetMetadata->Name] = subAsset;
                else
                    materialsTableHandle = subAsset;
            }
        }

        if (!defaultShader)
        {
            Grapple_CORE_ERROR("Failed to find 'Mesh' shader");
            return;
        }

        std::optional<uint32_t> colorProperty;
        std::optional<uint32_t> roughnessProperty;
        std::optional<uint32_t> textureProperty;
        std::optional<uint32_t> normalMapProperty;
        std::optional<uint32_t> roughnessMapProperty;

        Ref<Shader> shader = AssetManager::GetAsset<Shader>(defaultShader.value());
        if (shader != nullptr && shader->IsLoaded())
        {
            colorProperty = shader->GetPropertyIndex("u_Material.Color");
            roughnessProperty = shader->GetPropertyIndex("u_Material.Roughness");
            textureProperty = shader->GetPropertyIndex("u_Texture");
            normalMapProperty = shader->GetPropertyIndex("u_NormalMap");
            roughnessMapProperty = shader->GetPropertyIndex("u_RoughnessMap");
        }

        Ref<MaterialsTable> materialsTable = CreateRef<MaterialsTable>();
        if (AssetManager::IsAssetHandleValid(materialsTableHandle))
            assetManager->SetLoadedAsset(materialsTableHandle, materialsTable);
        else
            assetManager->ImportMemoryOnlyAsset("DefaultMaterialsTable", materialsTable, metadata.Handle);

        materialsTable->Materials.reserve(usedMaterials.size());

        auto getMaterialTexture = [&](const aiMaterial& material, aiTextureType type) -> AssetHandle
		{
            aiTextureMapping mapping;
            uint32_t uvIndex;
			aiString path;

			aiReturn result = material.GetTexture(type, 0, &path, &mapping, &uvIndex);
            if (result == aiReturn_SUCCESS)
				return FindTextureByPath(std::string_view(path.C_Str(), path.length), metadata, assetManager);

            return NULL_ASSET_HANDLE;
		};

        for (uint32_t i : usedMaterials)
        {
            auto& material = scene->mMaterials[i];

            std::string name = material->GetName().C_Str();
            if (name.empty())
                name = fmt::format("Material {}", i);

            AssetHandle baseColorTextureHandle = NULL_ASSET_HANDLE;
            AssetHandle normalMapHandle = NULL_ASSET_HANDLE;
            AssetHandle roughnessMapHandle = NULL_ASSET_HANDLE;

            baseColorTextureHandle = getMaterialTexture(*material, aiTextureType_BASE_COLOR);
            normalMapHandle = getMaterialTexture(*material, aiTextureType_NORMALS);
            roughnessMapHandle = getMaterialTexture(*material, aiTextureType_DIFFUSE_ROUGHNESS);

            aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            float roughness = 1.0f;
            material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

            Ref<Material> materialAsset = Material::Create(defaultShader.value());

            if (colorProperty)
                materialAsset->WritePropertyValue(*colorProperty, glm::vec4(color.r, color.g, color.b, color.a));
            if (roughnessProperty)
                materialAsset->WritePropertyValue(*roughnessProperty, roughness);

            TrySetMaterialTexture(textureProperty, materialAsset, baseColorTextureHandle, Renderer::GetWhiteTexture());
            TrySetMaterialTexture(normalMapProperty, materialAsset, normalMapHandle, Renderer::GetDefaultNormalMap());
            TrySetMaterialTexture(roughnessMapProperty, materialAsset, roughnessMapHandle, Renderer::GetWhiteTexture());

            auto it = nameToHandle.find(name);
            if (it != nameToHandle.end())
            {
                assetManager->SetLoadedAsset(it->second, materialAsset);
                materialsTable->Materials.push_back(it->second);
            }
            else
            {
                AssetHandle handle = assetManager->ImportMemoryOnlyAsset(name, materialAsset, metadata.Handle);
                materialsTable->Materials.push_back(handle);
            }

        }
    }

    Ref<Mesh> MeshImporter::ImportMesh(const AssetMetadata& metadata)
    {
        std::underlying_type_t<aiPostProcessSteps> postProcessSteps = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder;
        if (metadata.Path.extension() == ".fbx")
            postProcessSteps |= aiProcess_FlipUVs;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(metadata.Path.string(), postProcessSteps);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Grapple_CORE_ERROR("Failed to load mesh {}: {}", metadata.Path.generic_string(), importer.GetErrorString());
            return nullptr;
        }

        std::vector<uint32_t> usedMaterials;

        Ref<Mesh> mesh = ProcessMeshNode(scene->mRootNode, scene, usedMaterials);
        ImportMaterials(metadata, scene, usedMaterials);

        return mesh;
    }
}
