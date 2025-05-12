#include "PrefabImporter.h"

#include "Grapple/Scene/Prefab.h"

#include "GrappleECS/World.h"

#include "GrappleEditor/Serialization/SceneSerializer.h"
#include "GrappleEditor/Serialization/YAMLSerialization.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <exception>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Grapple
{
    void PrefabImporter::SerializePrefab(AssetHandle prefab, World& world, Entity entity)
    {
        Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(prefab));

        if (!world.IsEntityAlive(entity))
            return;

        const AssetMetadata* metadata = AssetManager::GetAssetMetadata(prefab);
        
        YAML::Emitter emitter;
        SceneSerializer::SerializeEntity(emitter, world, entity);

        std::ofstream output(metadata->Path);
        output << emitter.c_str();
        output.close();

        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
        assetManager->ReloadAsset(prefab);
    }

    Ref<Asset> PrefabImporter::ImportPrefab(const AssetMetadata& metadata)
    {
        try
        {
            YAML::Node node = YAML::LoadFile(metadata.Path.generic_string());
            
            World& world = World::GetCurrent();

            size_t prefabDataSize = 0;
            std::vector<std::pair<ComponentId, void*>> components;

            if (YAML::Node componentsNode = node["Components"])
            {
                for (YAML::Node componentNode : componentsNode)
                {
                    if (YAML::Node nameNode = componentNode["Name"])
                    {
                        std::string name = nameNode.as<std::string>();
                        auto componentId = world.Components.FindComponnet(name);

                        if (!componentId.has_value())
                            continue;

                        if (componentId.has_value())
                        {
                            prefabDataSize += world.Components.GetComponentInfo(componentId.value()).Size;
                            components.push_back({ componentId.value(), nullptr });
                        }
                    }
                }
            }

            uint8_t* prefabData = nullptr;
            if (prefabDataSize != 0)
            {
                prefabData = new uint8_t[prefabDataSize];
                std::memset(prefabData, 0, prefabDataSize);

                size_t writeOffset = 0;
                size_t index = 0;

                if (YAML::Node componentsNode = node["Components"])
                {
                    for (YAML::Node componentNode : componentsNode)
                    {
                        YAML::Node nameNode = componentNode["Name"];
                        if (!nameNode)
                            continue;

						std::string name = nameNode.as<std::string>();
						auto componentId = world.Components.FindComponnet(name);
                        if (!componentId)
                            continue;

						const ComponentInfo& info = world.Components.GetComponentInfo(componentId.value());

                        YAML::Node dataNode = componentNode["Data"];
						if (info.Initializer && dataNode)
						{
							YAMLDeserializer deserializer(componentNode);
							deserializer.PropertyKey("Data");
							deserializer.SerializeObject(info.Initializer->Type.SerializationDescriptor, prefabData + writeOffset);
						}

						components[index].second = (void*)(prefabData + writeOffset);

						writeOffset += info.Size;
						index++;
                    }
                }
            }

            using Pair = std::pair<ComponentId, void*>;
            std::sort(components.begin(), components.end(), [](const Pair& a, const Pair& b) -> bool { return a.first < b.first; });

            return CreateRef<Prefab>(prefabData, std::move(components));
        }
        catch (std::exception& e)
        {
            Grapple_CORE_ERROR("Failed to import prefab '{0}': {1}", metadata.Path.generic_string(), e.what());
        }

        return nullptr;
    }
}
