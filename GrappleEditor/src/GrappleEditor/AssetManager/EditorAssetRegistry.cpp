#include "EditorAssetRegistry.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Project/Project.h"
#include "Grapple/Serialization/Serialization.h"

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
    const std::filesystem::path AssetRegistrySerializer::RegistryFileName = "AssetRegistry.yaml";
    const std::filesystem::path AssetRegistrySerializer::AssetsDirectoryName = "Assets";

	void Grapple::AssetRegistrySerializer::Serialize(const EditorAssetRegistry& registry, const std::filesystem::path& path)
	{
        Grapple_PROFILE_FUNCTION();

        std::filesystem::path registryPath = path / RegistryFileName;
        std::filesystem::path root = path / AssetsDirectoryName;

        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetRegistry" << YAML::Value << YAML::BeginSeq; // Asset Registry

        for (const auto& [handle, entry] : registry)
        {
            std::filesystem::path assetPath = std::filesystem::relative(entry.Metadata.Path, root);

            emitter << YAML::BeginMap;
            emitter << YAML::Key << "Handle" << YAML::Value << handle;
            emitter << YAML::Key << "Type" << YAML::Value << AssetTypeToString(entry.Metadata.Type);
            emitter << YAML::Key << "BuiltIn" << YAML::Value << entry.IsBuiltIn;

            if (entry.Metadata.Source == AssetSource::Memory)
                emitter << YAML::Key << "Name" << YAML::Value << entry.Metadata.Name;
            else
                emitter << YAML::Key << "Path" << YAML::Value << assetPath.generic_string();

            emitter << YAML::Key << "Source" << YAML::Value << AssetSourceToString(entry.Metadata.Source);
            emitter << YAML::Key << "Parent" << YAML::Value << entry.Metadata.Parent;
            emitter << YAML::Key << "SubAssets" << YAML::Value << YAML::BeginSeq; // Sub assets

            for (AssetHandle subAsset : entry.Metadata.SubAssets)
                emitter << YAML::Value << subAsset;

            emitter << YAML::EndSeq;

            emitter << YAML::EndMap;
        }

        emitter << YAML::EndSeq; // Asset Registry
        emitter << YAML::EndMap;

        std::ofstream outputFile(registryPath);
        outputFile << emitter.c_str();
	}

    bool AssetRegistrySerializer::Deserialize(EditorAssetRegistry& registry, const std::filesystem::path& path)
    {
        Grapple_PROFILE_FUNCTION();

        std::filesystem::path registryPath = path / RegistryFileName;
        std::filesystem::path root = path / AssetsDirectoryName;

        std::ifstream inputFile(registryPath);
        if (!inputFile)
        {
            Grapple_CORE_ERROR("Failed to read asset registry file: {0}", registryPath.string());
            return false;
        }

        YAML::Node node = YAML::Load(inputFile);
        YAML::Node registryNode = node["AssetRegistry"];

        if (!registryNode)
            return false;

        for (auto assetNode : registryNode)
        {
            AssetHandle handle = assetNode["Handle"].as<AssetHandle>();
            
            AssetRegistryEntry entry{};
            AssetMetadata& metadata = entry.Metadata;
            metadata.Handle = handle;
            metadata.Source = AssetSource::File;
            metadata.Type = AssetTypeFromString(assetNode["Type"].as<std::string>());

            if (YAML::Node source = assetNode["Source"])
                metadata.Source = AssetSourceFromString(source.as<std::string>());

            if (YAML::Node builtIn = assetNode["BuiltIn"])
                entry.IsBuiltIn = builtIn.as<bool>();

            if (metadata.Source == AssetSource::Memory)
            {
                if (YAML::Node name = assetNode["Name"])
                    metadata.Name = name.as<std::string>();
            }
            else
            {
                if (YAML::Node pathNode = assetNode["Path"])
                    metadata.Path = root / std::filesystem::path(pathNode.as<std::string>());

                metadata.Name = metadata.Path.filename().generic_string();
            }

            if (YAML::Node parent = assetNode["Parent"])
                metadata.Parent = parent.as<AssetHandle>();
            else
                metadata.Parent = NULL_ASSET_HANDLE;

            if (YAML::Node subAssets = assetNode["SubAssets"])
            {
                for (YAML::Node subAsset : subAssets)
                    metadata.SubAssets.push_back(subAsset.as<AssetHandle>());
            }

            registry.emplace(handle, entry);
        }

        return true;
    }
}
