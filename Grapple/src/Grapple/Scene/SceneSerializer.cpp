#include "SceneSerializer.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Serialization/Serialization.h"

#include "Grapple/Scripting/ScriptingEngine.h"

#include "GrappleECS/Query/EntityRegistryIterator.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include <fstream>

namespace Grapple
{
	static void SerializeType(YAML::Emitter& emitter, const Scripting::ScriptingType& type, const uint8_t* data)
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Name" << YAML::Value << std::string(type.Name);

		const Scripting::TypeSerializationSettings& serializationSettings = type.GetSerializationSettings();
		for (const Scripting::Field& field : serializationSettings.GetFields())
		{
			const uint8_t* fieldData = (const uint8_t*)data + field.Offset;
			switch (field.Type)
			{
			case Scripting::FieldType::Bool:
				emitter << YAML::Key << field.Name << YAML::Value << *(bool*)(fieldData);
				break;
			case Scripting::FieldType::Float:
				emitter << YAML::Key << field.Name << YAML::Value << *(float*)(fieldData);
				break;
			case Scripting::FieldType::Float2:
				emitter << YAML::Key << field.Name << YAML::Value << *(glm::vec2*)(fieldData);
				break;
			case Scripting::FieldType::Float3:
				emitter << YAML::Key << field.Name << YAML::Value << *(glm::vec3*)(fieldData);
				break;
			case Scripting::FieldType::Asset:
			case Scripting::FieldType::Texture:
				emitter << YAML::Key << field.Name << YAML::Value << *(AssetHandle*)(fieldData);
				break;
			case Scripting::FieldType::Entity:
			{
				Grapple_CORE_WARN("Entity serialization is not implemented");
				break;
			}
			default:
				Grapple_CORE_ASSERT(false, "Unhandled field type");
			}
		}

		emitter << YAML::EndMap;
	}

	static void DeserializeType(YAML::Node& node, const Scripting::ScriptingType& type, uint8_t* data)
	{
		for (const Scripting::Field& field : type.GetSerializationSettings().GetFields())
		{
			if (YAML::Node fieldNode = node[field.Name])
			{
				switch (field.Type)
				{
				case Scripting::FieldType::Bool:
				{
					float value = fieldNode.as<bool>();
					std::memcpy(data + field.Offset, &value, sizeof(value));
					break;
				}
				case Scripting::FieldType::Float:
				{
					float value = fieldNode.as<float>();
					std::memcpy(data + field.Offset, &value, sizeof(value));
					break;
				}
				case Scripting::FieldType::Float2:
				{
					glm::vec2 vector = fieldNode.as<glm::vec2>();
					std::memcpy(data + field.Offset, &vector, sizeof(vector));
					break;
				}
				case Scripting::FieldType::Float3:
				{
					glm::vec3 vector = fieldNode.as<glm::vec3>();
					std::memcpy(data + field.Offset, &vector, sizeof(vector));
					break;
				}
				case Scripting::FieldType::Asset:
				case Scripting::FieldType::Texture:
				{
					AssetHandle handle = fieldNode.as<AssetHandle>();
					std::memcpy(data + field.Offset, &handle, sizeof(handle));
					break;
				}
				case Scripting::FieldType::Entity:
				{
					Grapple_CORE_WARN("Entity serialization is not implemented");
					break;
				}
				default:
					Grapple_CORE_ASSERT(false, "Unhandled field type");
				}
			}
		}
	}

	static void SerializeComponent(YAML::Emitter& emitter, World& world, Entity entity, ComponentId component)
	{
		if (component == TransformComponent::Info.Id)
		{
			TransformComponent& transform = world.GetEntityComponent<TransformComponent>(entity);

			emitter << YAML::BeginMap << YAML::Key << "Name" << YAML::Value << "Transform";

			emitter << YAML::Key << "Position" << YAML::Value << transform.Position;
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			emitter << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			emitter << YAML::EndMap;
		}
		else if (component == SpriteComponent::Info.Id)
		{
			SpriteComponent& sprite = world.GetEntityComponent<SpriteComponent>(entity);

			emitter << YAML::BeginMap << YAML::Key << "Name" << YAML::Value << "Sprite";
			emitter << YAML::Key << "Color" << YAML::Value << sprite.Color;
			emitter << YAML::Key << "Texture" << YAML::Value << sprite.Texture;
			emitter << YAML::Key << "TextureTiling" << YAML::Value << sprite.TextureTiling;
			emitter << YAML::EndMap;
		}
		else if (component == CameraComponent::Info.Id)
		{
			CameraComponent& camera = world.GetEntityComponent<CameraComponent>(entity);

			emitter << YAML::BeginMap << YAML::Key << "Name" << YAML::Value << "Camera";

			emitter << YAML::Key << "Size" << YAML::Value << camera.Size;
			emitter << YAML::Key << "Near" << YAML::Value << camera.Near;
			emitter << YAML::Key << "Far" << YAML::Value << camera.Far;

			emitter << YAML::Key << "Projection";
			switch (camera.Projection)
			{
			case CameraComponent::ProjectionType::Orthographic:
				emitter << YAML::Value << "Orthographic";
				break;
			case CameraComponent::ProjectionType::Perspective:
				emitter << YAML::Value << "Perspective";
				break;
			}

			emitter << YAML::Key << "FOV" << YAML::Value << camera.FOV;
			emitter << YAML::EndMap;
		}
		else
		{
		 	std::optional<const Scripting::ScriptingType*> componentType = ScriptingEngine::FindComponentType(component);
			std::optional<void*> entityData = world.GetRegistry().GetEntityComponent(entity, component);

			Grapple_CORE_ASSERT(entityData.has_value());

			if (componentType.has_value())
				SerializeType(emitter, *componentType.value(), (const uint8_t*)entityData.value());
			else
				Grapple_CORE_ERROR("Componnet with id={{0};{1}} cannot be serialized because it's type infomation cannot be found",
					component.GetIndex(), component.GetGeneration());
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
			world.GetRegistry().AddEntityComponent(entity, id, nullptr);
			return (uint8_t*) world.GetRegistry().GetEntityComponent(entity, id).value();
		}
		else
		{
			entity = world.GetRegistry().CreateEntity(ComponentSet(&id, 1));
			return (uint8_t*) world.GetRegistry().GetEntityComponent(entity, id).value();
		}
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene)
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(scene->Handle));
		Serialize(scene, AssetManager::GetAssetMetadata(scene->Handle)->Path);
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entities";
		emitter << YAML::BeginSeq;

		for (Entity entity : scene->m_World.GetRegistry())
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Components" << YAML::BeginSeq; // Components

			for (ComponentId component : scene->m_World.GetEntityComponents(entity))
				SerializeComponent(emitter, scene->m_World, entity, component);

			emitter << YAML::EndSeq; // Components
			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Systems";
		emitter << YAML::BeginSeq;

		const auto& modules = ScriptingEngine::GetData().Modules;
		for (const ScriptingModuleData& module : modules)
		{
			for (size_t systemIndex = module.FirstSystemInstance; systemIndex < module.Config.RegisteredSystems->size(); systemIndex++)
			{
				const ScriptingTypeInstance& instance = module.ScriptingInstances[systemIndex];
				std::optional<const Scripting::ScriptingType*> type = ScriptingEngine::GetScriptingType(instance.Type);

				if (type.has_value())
					SerializeType(emitter, *type.value(), (const uint8_t*)instance.Instance);
			}
		}
		emitter << YAML::EndSeq;

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

	void SceneSerializer::Deserialize(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		std::ifstream inputFile(path);
		if (!inputFile)
		{
			Grapple_CORE_ERROR("Failed to read scene file: {0}", path.string());
			return;
		}

		YAML::Node node = YAML::Load(inputFile);
		inputFile.close();

		YAML::Node entities = node["Entities"];
		if (!entities)
			return;

		for (auto entity : entities)
		{
			YAML::Node componentsNode = entity["Components"];

			Entity entity;
			for (YAML::Node componentNode : componentsNode)
			{
				std::string name = componentNode["Name"].as<std::string>();

				if (name == "Transform")
				{
					YAML::Node positionNode = componentNode["Position"];
					
					TransformComponent transformComponent;
					transformComponent.Position = positionNode.as<glm::vec3>();
					transformComponent.Rotation = componentNode["Rotation"].as<glm::vec3>();
					transformComponent.Scale = componentNode["Scale"].as<glm::vec3>();

					AddDeserializedComponent<TransformComponent>(scene->m_World, entity, transformComponent);
				}
				else if (name == "Sprite")
				{
					SpriteComponent spriteComponent;
					spriteComponent.Color = componentNode["Color"].as<glm::vec4>();

					if (YAML::Node textureNode = componentNode["Texture"])
						spriteComponent.Texture = textureNode ? textureNode.as<uint64_t>() : NULL_ASSET_HANDLE;

					if (YAML::Node tilingNode = componentNode["TextureTiling"])
						spriteComponent.TextureTiling = tilingNode.as<glm::vec2>();
					else
						spriteComponent.TextureTiling = glm::vec2(1.0f);

					AddDeserializedComponent<SpriteComponent>(scene->m_World, entity, spriteComponent);
				}
				else if (name == "Camera")
				{
					CameraComponent cameraComponent;
					cameraComponent.Size = componentNode["Size"].as<float>();
					cameraComponent.Near = componentNode["Near"].as<float>();
					cameraComponent.Far = componentNode["Far"].as<float>();

					if (YAML::Node fovNode = componentNode["FOV"])
						cameraComponent.FOV = fovNode.as<float>();

					if (YAML::Node projectionType = componentNode["Projection"])
					{
						std::string string = projectionType.as<std::string>();
						if (string == "Orthographic")
							cameraComponent.Projection = CameraComponent::ProjectionType::Orthographic;
						else if (string == "Perspective")
							cameraComponent.Projection = CameraComponent::ProjectionType::Perspective;
					}

					AddDeserializedComponent<CameraComponent>(scene->m_World, entity, cameraComponent);
				}
				else
				{
					std::optional<ComponentId> componentId = scene->GetECSWorld().GetRegistry().FindComponnet(name);
					if (!componentId.has_value())
					{
						Grapple_CORE_ERROR("Component named '{0}' cannot not be deserialized", name);
						continue;
					}

					std::optional<const Scripting::ScriptingType*> type = ScriptingEngine::FindComponentType(componentId.value());
					if (type.has_value())
					{
						uint8_t* componentData = AddDeserializedComponent(scene->GetECSWorld(), entity, componentId.value());
						DeserializeType(componentNode, *type.value(), componentData);
					}
				}
			}
		}

		YAML::Node systems = node["Systems"];
		if (!systems)
			return;

		for (auto system : systems)
		{
			if (YAML::Node nameNode = system["Name"])
			{
				std::string name = nameNode.as<std::string>();
				std::optional<const Scripting::ScriptingType*> type = ScriptingEngine::FindType(name);
				if (!type.has_value())
				{
					Grapple_CORE_ERROR("System named '{0}' cannot be deserialized", name);
					continue;
				}

				std::optional<Scripting::SystemBase*> systemData = ScriptingEngine::FindSystemByName(name);
				if (!systemData.has_value())
				{
					Grapple_CORE_ERROR("System '{0}' cannot be found", name);
					continue;
				}

				DeserializeType(system, *type.value(), (uint8_t*) systemData.value());
			}
		}
	}
}