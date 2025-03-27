#include "EditorAssetManager.h"

#include "Grapple/Core/Assert.h"
#include "Grapple/Core/Log.h"

#include "Grapple/AssetManager/Importers/TextureImporter.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <string>

namespace YAML
{
    Emitter& operator<<(Emitter& emitter, std::string_view string)
    {
        emitter << std::string(string.data(), string.size());
        return emitter;
    }
}

namespace Grapple
{
    EditorAssetManager::EditorAssetManager(const std::filesystem::path& root)
        : m_Root(root)
    {
        m_AssetImporters.emplace(AssetType::Texture, TextureImporter::ImportTexture);
        DeserializeRegistry();
    }

    Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
    {
        return Ref<Asset>();
    }

    bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        return false;
    }

    bool EditorAssetManager::IsAssetLoaded(AssetHandle handle)
    {
        return false;
    }

    std::optional<AssetHandle> EditorAssetManager::FindAssetByPath(const std::filesystem::path& path)
    {
        auto it = m_FilepathToAssetHandle.find(path);
        if (it == m_FilepathToAssetHandle.end())
            return {};
        return it->second;
    }

    bool EditorAssetManager::ImportAsset(const std::filesystem::path& path)
    {
        AssetType type = AssetType::None;
        std::filesystem::path extension = path.extension();

        if (extension == ".png")
            type = AssetType::Texture;

        auto importerIterator = m_AssetImporters.find(type);
        if (importerIterator == m_AssetImporters.end())
        {
            Grapple_CORE_WARN("Cannot import '{0}', because an imported for that asset type is not provided");
            return false;
        }

        AssetHandle handle;
        AssetMetadata metadata;
        metadata.Handle = handle;
        metadata.Path = path;
        metadata.Type = type;

        Ref<Asset> asset = importerIterator->second(metadata);

        m_LoadedAssets.emplace(handle, asset);
        m_Registry.emplace(handle, metadata);
        m_FilepathToAssetHandle.emplace(path, handle);

        SerializeRegistry();
    }

    void EditorAssetManager::SerializeRegistry()
    {
        std::filesystem::path path = m_Root / std::filesystem::path("AssetRegistry.yaml");

        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetRegistry" << YAML::Value << YAML::BeginSeq; // Asset Registry

        for (const auto& [handle, metadata] : m_Registry)
        {
            std::filesystem::path assetPath = std::filesystem::relative(metadata.Path, m_Root);

            emitter << YAML::BeginMap;
            emitter << YAML::Key << "Handle" << YAML::Value << (uint64_t)handle;
            emitter << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            emitter << YAML::Key << "Path" << YAML::Value << assetPath.generic_string();
            emitter << YAML::EndMap;
        }

        emitter << YAML::EndSeq; // Asset Registry
        emitter << YAML::EndMap;

        std::ofstream outputFile(path);
        outputFile << emitter.c_str();
    }

    void EditorAssetManager::DeserializeRegistry()
    {
        std::filesystem::path registryPath = m_Root / std::filesystem::path("AssetRegistry.yaml");

        std::ifstream inputFile(registryPath);
        if (!inputFile)
        {
            Grapple_CORE_ERROR("Failed to read scene file: {0}", registryPath.string());
            return;
        }

        YAML::Node node = YAML::Load(inputFile);

        YAML::Node registry = node["AssetRegistry"];
        if (!registry)
            return;

        for (auto assetNode : registry)
        {
            AssetHandle handle = assetNode["Handle"].as<uint64_t>();
            std::filesystem::path path = m_Root / std::filesystem::path(assetNode["Path"].as<std::string>());

            AssetType type = AssetType::None;
            if (assetNode["Type"].as<std::string>() == "Texture")
                type = AssetType::Texture;

            Grapple_CORE_ASSERT(type != AssetType::None);

            AssetMetadata metadata;
            metadata.Handle = handle;
            metadata.Path = path;
            metadata.Type = type;

            m_Registry.emplace(handle, metadata);
            m_FilepathToAssetHandle.emplace(path, handle);
        }
    }
}
