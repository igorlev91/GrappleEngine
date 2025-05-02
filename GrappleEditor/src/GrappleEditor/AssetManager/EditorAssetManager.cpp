#include "EditorAssetManager.h"

#include "GrappleCore/Assert.h"
#include "GrappleCore/Log.h"

#include "Grapple/Serialization/Serialization.h"
#include "Grapple/Project/Project.h"

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/Font.h"

#include "GrappleEditor/EditorLayer.h"

#include "GrappleEditor/AssetManager/TextureImporter.h"
#include "GrappleEditor/AssetManager/PrefabImporter.h"
#include "GrappleEditor/Serialization/SceneSerializer.h"
#include "GrappleEditor/AssetManager/MaterialImporter.h"
#include "GrappleEditor/AssetManager/MeshImporter.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <string>

namespace Grapple
{
    EditorAssetManager::EditorAssetManager()
    {
        m_AssetImporters.emplace(AssetType::Texture, TextureImporter::ImportTexture);
        m_AssetImporters.emplace(AssetType::Prefab, PrefabImporter::ImportPrefab);
        m_AssetImporters.emplace(AssetType::Material, MaterialImporter::ImportMaterial);
        m_AssetImporters.emplace(AssetType::Mesh, MeshImporter::ImportMesh);
        m_AssetImporters.emplace(AssetType::Scene, [](const AssetMetadata& metadata) -> Ref<Asset>
        {
            Ref<Scene> scene = CreateRef<Scene>(EditorLayer::GetInstance().GetECSContext());
            SceneSerializer::Deserialize(scene, metadata.Path);
            return scene;
        });

        m_AssetImporters.emplace(AssetType::Shader, [](const AssetMetadata& metadata) -> Ref<Asset>
        {
            std::filesystem::path assetsPath = Project::GetActive()->Location / "Assets";
            std::filesystem::path cacheDirectory = Project::GetActive()->Location
                / "Cache/Shaders/"
                / std::filesystem::relative(metadata.Path.parent_path(), assetsPath);
            return Shader::Create(metadata.Path, cacheDirectory);
        });

        m_AssetImporters.emplace(AssetType::Font, [](const AssetMetadata& metadata) -> Ref<Asset>
        {
            return CreateRef<Font>(metadata.Path);
        });
    }

    void EditorAssetManager::Reinitialize()
    {
        m_LoadedAssets.clear();
        m_Registry.clear();
        m_FilepathToAssetHandle.clear();

        Grapple_CORE_ASSERT(Project::GetActive());

        std::filesystem::path root = AssetRegistrySerializer::GetAssetsRoot();
        if (!std::filesystem::exists(root))
            std::filesystem::create_directories(root);

        DeserializeRegistry();
    }

    Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
    {
        auto assetIterator = m_LoadedAssets.find(handle);
        if (assetIterator != m_LoadedAssets.end())
            return assetIterator->second;

        auto registryIterator = m_Registry.find(handle);
        if (registryIterator == m_Registry.end())
            return nullptr;

        return LoadAsset(registryIterator->second.Metadata);
    }

    const AssetMetadata* EditorAssetManager::GetAssetMetadata(AssetHandle handle)
    {
        auto it = m_Registry.find(handle);
        if (it == m_Registry.end())
            return nullptr;
        return &it->second.Metadata;
    }

    bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        return m_Registry.find(handle) != m_Registry.end();
    }

    bool EditorAssetManager::IsAssetLoaded(AssetHandle handle)
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }

    std::optional<AssetHandle> EditorAssetManager::FindAssetByPath(const std::filesystem::path& path)
    {
        auto it = m_FilepathToAssetHandle.find(path);
        if (it == m_FilepathToAssetHandle.end())
            return {};
        return it->second;
    }

    AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path& path)
    {
        AssetType type = AssetType::None;
        std::filesystem::path extension = path.extension();

        if (extension == ".png")
            type = AssetType::Texture;
        else if (extension == ".Grapple")
            type = AssetType::Scene;
        else if (extension == ".flrprefab")
            type = AssetType::Prefab;
        else if (extension == ".flrmat")
            type = AssetType::Material;
        else if (extension == ".glsl")
            type = AssetType::Shader;
        else if (extension == ".ttf")
            type = AssetType::Font;
        else if (extension == ".fbx" || extension == ".gltf")
            type = AssetType::Mesh;

        AssetHandle handle;

        AssetRegistryEntry entry;
        entry.OwnerType = AssetOwner::Project;
        entry.PackageId = {};

        AssetMetadata& metadata = entry.Metadata;
        metadata.Path = path;
        metadata.Type = type;
        metadata.Handle = handle;

        m_Registry.emplace(handle, entry);
        m_FilepathToAssetHandle.emplace(path, handle);

        SerializeRegistry();

        return handle;
    }

    AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path& path, const Ref<Asset> asset)
    {
        AssetHandle handle;

        AssetRegistryEntry entry;
        entry.OwnerType = AssetOwner::Project;
        entry.PackageId = {};

        AssetMetadata& metadata = entry.Metadata;
        metadata.Handle = handle;
        metadata.Path = path;
        metadata.Type = asset->GetType();

        asset->Handle = handle;

        m_Registry.emplace(handle, entry);
        m_LoadedAssets.emplace(asset->Handle, asset);
        m_FilepathToAssetHandle.emplace(path, handle);

        SerializeRegistry();

        return handle;
    }

    void EditorAssetManager::ReloadAsset(AssetHandle handle)
    {
        Grapple_CORE_ASSERT(IsAssetHandleValid(handle));

        auto registryIterator = m_Registry.find(handle);
        if (registryIterator == m_Registry.end())
            return;

        LoadAsset(registryIterator->second.Metadata);
    }

    void EditorAssetManager::UnloadAsset(AssetHandle handle)
    {
        auto it = m_LoadedAssets.find(handle);
        if (it == m_LoadedAssets.end())
            return;

        m_LoadedAssets.erase(it);
    }

    void EditorAssetManager::ReloadPrefabs()
    {
        for (const auto& [handle, asset] : m_Registry)
        {
            if (asset.Metadata.Type == AssetType::Prefab && IsAssetLoaded(handle))
                ReloadAsset(handle);
        }
    }

    void EditorAssetManager::RemoveFromRegistry(AssetHandle handle)
    {
        auto it = m_Registry.find(handle);
        if (it != m_Registry.end())
        {
            UnloadAsset(handle);
            m_Registry.erase(handle);
            SerializeRegistry();
        }
    }

    Ref<Asset> EditorAssetManager::LoadAsset(const AssetMetadata& metadata)
    {
        auto importerIterator = m_AssetImporters.find(metadata.Type);
        if (importerIterator == m_AssetImporters.end())
        {
            Grapple_CORE_ASSERT("Cannot import '{0}', because an imported for that asset type is not provided");
            return nullptr;
        }

        Ref<Asset> asset = importerIterator->second(metadata);
        asset->Handle = metadata.Handle;
        m_LoadedAssets[metadata.Handle] = asset;
        return asset;
    }

    void EditorAssetManager::SerializeRegistry()
    {
        AssetRegistrySerializer::Serialize(m_Registry, Project::GetActive()->Location);
    }

    void EditorAssetManager::DeserializeRegistry()
    {
        m_Registry.clear();
        AssetRegistrySerializer::Deserialize(m_Registry, Project::GetActive()->Location);

        for (const auto& [handle, entry] : m_Registry)
            m_FilepathToAssetHandle.emplace(entry.Metadata.Path, handle);
    }
}
