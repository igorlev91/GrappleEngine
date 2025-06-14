#include "SceneSerializer.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Serialization/Serialization.h"
#include "GrappleCore/Serialization/TypeInitializer.h"

#include "GrappleECS/Query/EntitiesIterator.h"

#include "GrappleEditor/Serialization/SerializationId.h"
#include "GrappleEditor/Serialization/YAMLSerialization.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include <fstream>

namespace Grapple
{
	void SceneSerializer::SerializeComponent(YAML::Emitter& emitter, const World& world, Entity entity, ComponentId component)
	{
		std::optional<const void*> entityData = world.Entities.GetEntityComponent(entity, component);
		const ComponentInfo& info = world.Components.GetComponentInfo(component);

		if (entityData.has_value() && info.Initializer)
		{
			YAMLSerializer serializer(emitter, &world);
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << info.Initializer->Type.SerializationDescriptor.Name;

			emitter << YAML::Key << "Data" << YAML::Value;
			serializer.SerializeObject(info.Initializer->Type.SerializationDescriptor, (void*)entityData.value(), false, 0);
			emitter << YAML::EndMap;
		}
	}

	template<typename T>
	static void AddDeserializedComponent(World& world, Entity& entity, const T& componentData)
	{
		if (world.IsEntityAlive(entity))
			world.AddEntityComponent<T>(entity, componentData);
		else
		{
			entity = world.CreateEntity<T>();
			T& component = world.GetEntityComponent<T>(entity);
			component = componentData;
		}
	}

	static uint8_t* AddDeserializedComponent(World& world, Entity& entity, ComponentId id)
	{
		if (world.IsEntityAlive(entity))
		{
			bool result = world.Entities.AddEntityComponent(entity, id, nullptr);
			Grapple_CORE_ASSERT(result);

			return (uint8_t*)world.Entities.GetEntityComponent(entity, id);
		}
		else
		{
			entity = world.Entities.CreateEntity(ComponentSet(&id, 1));
			return (uint8_t*)world.Entities.GetEntityComponent(entity, id);
		}
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, World& world, Entity entity)
	{
		emitter << YAML::BeginMap;

		{
			const SerializationId* serializationId = world.TryGetEntityComponent<SerializationId>(entity);
			emitter << YAML::Key << "SerializationId" << YAML::Value;

			if (serializationId)
				emitter << serializationId->Id;
			else
				emitter << UUID();
		}

		emitter << YAML::Key << "Components" << YAML::BeginSeq; // Components

		for (ComponentId component : world.GetEntityComponents(entity))
		{
			if (component == COMPONENT_ID(SerializationId))
				continue;

			SerializeComponent(emitter, world, entity, component);
		}

		emitter << YAML::EndSeq; // Components
		emitter << YAML::EndMap;
	}

	static void DeserializeEntitySerializationId(const YAML::Node& node, World& world, Entity& outEntity, UUID& outSerializationId)
	{
		UUID id;
		const SerializableObjectDescriptor& idComponentDescriptor = Grapple_SERIALIZATION_DESCRIPTOR_OF(SerializationId);
		if (YAML::Node idNode = node["SerializationId"])
		{
			id = idNode.as<UUID>();
		}

		outEntity = world.CreateEntity<SerializationId>(SerializationId(id));
		outSerializationId = id;
	}

	void SceneSerializer::DeserializeEntity(Entity entity, const YAML::Node& node, World& world, std::unordered_map<UUID, Entity>& serializationIdToECSId)
	{
		YAML::Node componentsNode = node["Components"];
		if (!componentsNode)
			return;

		for (YAML::Node componentNode : componentsNode)
		{
			std::string name;
			if (YAML::Node nameNode = componentNode["Name"])
				name = nameNode.as<std::string>();
			else
				continue;

			std::optional<ComponentId> componentId = world.Components.FindComponnet(name);

			if (!componentId.has_value())
			{
				Grapple_CORE_ERROR("Component named '{0}' cannot be deserialized", name);
				continue;
			}

			if (componentId.value() == COMPONENT_ID(SerializationId))
				continue;

			const ComponentInfo& info = world.Components.GetComponentInfo(componentId.value());
			if (info.Initializer)
			{
				const SerializableObjectDescriptor& serializationDescriptor = info.Initializer->Type.SerializationDescriptor;
				uint8_t* componentData = AddDeserializedComponent(world, entity, componentId.value());

				YAML::Node componentDataNode = componentNode["Data"];
				if (componentDataNode && !componentDataNode.IsNull())
				{
					YAMLDeserializer deserializer(componentNode, &serializationIdToECSId);
					deserializer.PropertyKey("Data");
					deserializer.SerializeObject(serializationDescriptor, (void*)componentData, false, 0);
				}
			}
		}
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene, const EditorCamera& editorCamera, const SceneViewSettings& sceneViewSettings)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(scene->Handle));
		Serialize(scene, AssetManager::GetAssetMetadata(scene->Handle)->Path, editorCamera, sceneViewSettings);
	}

	void SerializePostProcessing(YAML::Emitter& emitter, Ref<Scene> scene)
	{
		const auto& postProcessingManager = scene->GetPostProcessingManager();

		emitter << YAML::Key << "PostProcessing" << YAML::BeginSeq;

		for (const auto& entry : postProcessingManager.GetEntries())
		{
			const SerializableObjectDescriptor& descriptor = *entry.Descriptor;

			emitter << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << descriptor.Name;
			emitter << YAML::Key << "Enabled" << YAML::Value << entry.Effect->IsEnabled();

			YAMLSerializer serializer(emitter, &scene->GetECSWorld());
			serializer.PropertyKey("Data");
			serializer.SerializeObject(descriptor, entry.Effect.get(), false, 0);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap; // PostProcessing
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::filesystem::path& path, const EditorCamera& editorCamera, const SceneViewSettings& sceneViewSettings)
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entities";
		emitter << YAML::BeginSeq;

		for (Entity entity : scene->m_World.Entities)
			SerializeEntity(emitter, scene->m_World, entity);

		emitter << YAML::EndSeq; // Entities

		{
			emitter << YAML::Key << "Editor" << YAML::BeginMap; // Editor

			emitter << YAML::Key << "Camera" << YAML::BeginMap; // Camera
			emitter << YAML::Key << "RotationOrigin" << YAML::Value << editorCamera.GetRotationOrigin();
			emitter << YAML::Key << "Rotation" << YAML::Value << editorCamera.GetRotation();
			emitter << YAML::Key << "Zoom" << YAML::Value << editorCamera.GetZoom();
			emitter << YAML::EndMap; // Camera

			emitter << YAML::Key << "SceneViewSettings" << YAML::BeginMap; // SceneViewSettings
			emitter << YAML::Key << "ShowAABBs" << YAML::Value << sceneViewSettings.ShowAABBs;
			emitter << YAML::Key << "ShowCameraFrustum" << YAML::Value << sceneViewSettings.ShowCameraFrustum;
			emitter << YAML::Key << "ShowLights" << YAML::Value << sceneViewSettings.ShowLights;
			emitter << YAML::Key << "ShowGrid" << YAML::Value << sceneViewSettings.ShowGrid;
			emitter << YAML::EndMap; // SceneViewSettings

			emitter << YAML::EndMap; // Editor
		}

		SerializePostProcessing(emitter, scene);

		emitter << YAML::EndMap;

		std::ofstream output(path);
		if (!output.is_open())
		{
			Grapple_CORE_ERROR("Failed to write scene file: {0}", path.string());
			return;
		}

		output << emitter.c_str();
		output.close();
	}

	bool DeserializePostProcessing(Ref<Scene> scene, const YAML::Node& node)
	{
		YAML::Node postProcessingList = node["PostProcessing"];

		if (!postProcessingList)
			return false;

		if (!postProcessingList.IsSequence())
			return false;

		const PostProcessingManager& postProcessingManager = scene->GetPostProcessingManager();
		const auto& entries = postProcessingManager.GetEntries();
		for (const auto& postProcessingNode : postProcessingList)
		{
			YAML::Node nameNode = postProcessingNode["Name"];
			if (!nameNode)
				continue;

			YAML::Node enabledNode = postProcessingNode["Enabled"];
			bool enabled = false;

			if (enabledNode)
				enabled = enabledNode.as<bool>();

			std::string effectName = nameNode.as<std::string>();

			const auto& entryIterator = std::find_if(
				entries.begin(),
				entries.end(),
				[&effectName](const PostProcessingManager::PostProcessingEntry& entry) -> bool
				{
					return entry.Descriptor->Name == effectName;
				});

			if (entryIterator == entries.end())
				continue;
			
			YAMLDeserializer deserializer(postProcessingNode);
			deserializer.PropertyKey("Data");
			deserializer.SerializeObject(*entryIterator->Descriptor, entryIterator->Effect.get(), false, 0);

			entryIterator->Effect->SetEnabled(enabled);
		}

		return true;
	}

	void SceneSerializer::Deserialize(const Ref<Scene>& scene, const std::filesystem::path& path, EditorCamera& editorCamera, SceneViewSettings& sceneViewSettings)
	{
		std::ifstream inputFile(path);
		if (!inputFile)
		{
			Grapple_CORE_ERROR("Failed to read scene file: {0}", path.string());
			return;
		}

		YAML::Node node;
		try
		{
			node = YAML::Load(inputFile);
		}
		catch (std::exception& e)
		{
			Grapple_CORE_ERROR(e.what());
			inputFile.close();
			return;
		}

		YAML::Node entities = node["Entities"];
		if (!entities)
			return;

		std::unordered_map<UUID, Entity> serializationIdToECSId;
		std::vector<Entity> entityIds;
		for (auto entity : entities)
		{
			Entity entityId;
			UUID serializationId;

			DeserializeEntitySerializationId(entity, scene->m_World, entityId, serializationId);

			serializationIdToECSId.emplace(serializationId, entityId);
			entityIds.push_back(entityId);
		}

		size_t index = 0;
		for (auto entityNode : entities)
		{
			DeserializeEntity(entityIds[index], entityNode, scene->m_World, serializationIdToECSId);
			index++;
		}

		DeserializePostProcessing(scene, node);

		YAML::Node editorSettings = node["Editor"];
		if (editorSettings)
		{
			if (YAML::Node camera = editorSettings["Camera"])
			{
				if (YAML::Node rotationOrigin = camera["RotationOrigin"])
					editorCamera.SetRotationOrigin(rotationOrigin.as<glm::vec3>());
				if (YAML::Node rotation = camera["Rotation"])
					editorCamera.SetRotation(rotation.as<glm::vec3>());
				if (YAML::Node zoom = camera["Zoom"])
					editorCamera.SetZoom(zoom.as<float>());
			}

			if (YAML::Node sceneSettings = editorSettings["SceneViewSettings"])
			{
				if (YAML::Node node = sceneSettings["ShowAABBs"])
					sceneViewSettings.ShowAABBs = node.as<bool>();
				if (YAML::Node node = sceneSettings["ShowCameraFrustum"])
					sceneViewSettings.ShowCameraFrustum = node.as<bool>();
				if (YAML::Node node = sceneSettings["ShowLights"])
					sceneViewSettings.ShowLights = node.as<bool>();
				if (YAML::Node node = sceneSettings["ShowGrid"])
					sceneViewSettings.ShowGrid = node.as<bool>();
			}
		}
	}
}