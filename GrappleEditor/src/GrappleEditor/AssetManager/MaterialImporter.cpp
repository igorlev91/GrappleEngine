#include "MaterialImporter.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/Shader.h"

#include "Grapple/Serialization/Serialization.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Grapple
{
	void MaterialImporter::SerializeMaterial(Ref<Material> material, const std::filesystem::path& path)
	{
		Ref<Shader> shader = material->GetShader();

		YAML::Emitter emitter;
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "Shader" << YAML::Value << shader->Handle;

		emitter << YAML::Key << "Properties" << YAML::BeginSeq;

		const ShaderProperties& properties = shader->GetProperties();
		for (uint32_t index = 0; index < (uint32_t)properties.size(); index++)
		{
			const ShaderProperty& parameter = properties[index];

			if (parameter.Type == ShaderDataType::Matrix4x4)
				continue;

			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Name" << YAML::Value << parameter.Name;
			emitter << YAML::Key << "Type" << YAML::Value << (uint32_t)parameter.Type;
			emitter << YAML::Key << "Value" << YAML::Value;

			switch (parameter.Type)
			{
			case ShaderDataType::Int:
				emitter << material->ReadPropertyValue<int32_t>(index);
				break;
			case ShaderDataType::Int2:
				emitter << material->ReadPropertyValue<glm::ivec2>(index);
				break;
			case ShaderDataType::Int3:
				emitter << material->ReadPropertyValue<glm::ivec3>(index);
				break;
			case ShaderDataType::Int4:
				emitter << material->ReadPropertyValue<glm::ivec4>(index);
				break;

			case ShaderDataType::Sampler:
				emitter << material->ReadPropertyValue<AssetHandle>(index);
				break;

			case ShaderDataType::Float:
				emitter << material->ReadPropertyValue<float>(index);
				break;
			case ShaderDataType::Float2:
				emitter << material->ReadPropertyValue<glm::vec2>(index);
				break;
			case ShaderDataType::Float3:
				emitter << material->ReadPropertyValue<glm::vec3>(index);
				break;
			case ShaderDataType::Float4:
				emitter << material->ReadPropertyValue<glm::vec4>(index);
				break;
			}

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq; // Parameters

		emitter << YAML::EndMap;

		std::ofstream output(path);
		output << emitter.c_str();
	}

	Ref<Material> MaterialImporter::ImportMaterial(const AssetMetadata& metadata)
	{
		if (metadata.Source == AssetSource::Memory)
		{
			As<EditorAssetManager>(AssetManager::GetInstance())->LoadAsset(metadata.Parent);
			return AssetManager::GetAsset<Material>(metadata.Handle);
		}

		Ref<Material> material = nullptr;

		try
		{
			std::optional<AssetHandle> shaderHandle;

			YAML::Node node = YAML::LoadFile(metadata.Path.generic_string());
			if (YAML::Node shaderNode = node["Shader"])
			{
				AssetHandle handle = shaderNode.as<AssetHandle>();
				if (!AssetManager::IsAssetHandleValid(handle))
				{
					Grapple_CORE_ERROR("Material asset has invalid shader handle");
				}
				else
				{
					material = CreateRef<Material>(handle);
					shaderHandle = handle;
				}
			}

			if (shaderHandle.has_value())
			{
				Ref<Shader> shader = AssetManager::GetAsset<Shader>(shaderHandle.value());
				if (!shader)
					return material;

				const ShaderProperties& shaderProperties = shader->GetProperties();

				if (YAML::Node parameters = node["Properties"])
				{
					for (YAML::Node parameter : parameters)
					{
						std::optional<uint32_t> index = {};
						std::optional<ShaderDataType> type = {};

						if (YAML::Node nameNode = parameter["Name"])
						{
							std::string name = nameNode.as<std::string>();
							index = shader->GetPropertyIndex(name);
						}

						if (YAML::Node typeNode = parameter["Type"])
						{
							uint32_t dataType = typeNode.as<uint32_t>();
							type = (ShaderDataType)dataType;
						}

						YAML::Node valueNode = parameter["Value"];
						if (index.has_value() && type.has_value() && valueNode)
						{
							switch (type.value())
							{
							case ShaderDataType::Int:
								material->WritePropertyValue(index.value(), valueNode.as<int32_t>());
								break;
							case ShaderDataType::Int2:
								material->WritePropertyValue(index.value(), valueNode.as<glm::ivec2>());
								break;
							case ShaderDataType::Int3:
								material->WritePropertyValue(index.value(), valueNode.as<glm::ivec3>());
								break;
							case ShaderDataType::Int4:
								material->WritePropertyValue(index.value(), valueNode.as<glm::ivec4>());
								break;

							case ShaderDataType::Sampler:
								material->WritePropertyValue(index.value(), valueNode.as<AssetHandle>());
								break;

							case ShaderDataType::Float:
								material->WritePropertyValue(index.value(), valueNode.as<float>());
								break;
							case ShaderDataType::Float2:
								material->WritePropertyValue(index.value(), valueNode.as<glm::vec2>());
								break;
							case ShaderDataType::Float3:
								material->WritePropertyValue(index.value(), valueNode.as<glm::vec3>());
								break;
							case ShaderDataType::Float4:
								auto a = valueNode.as<glm::vec4>();
								material->WritePropertyValue(index.value(), valueNode.as<glm::vec4>());
								break;
							}
						}
					}
				} // Parameters
			}
		}
		catch (std::exception& e)
		{
			Grapple_CORE_ERROR("Failed to import a material '{}': {}", metadata.Path.generic_string(), e.what());
		}

		return material;
	}
}
