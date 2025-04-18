#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/AssetManager/Asset.h"

#include <filesystem>

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Shader : public Asset
	{
	public:
		Shader()
			: Asset(AssetType::Shader) {}

		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, glm::vec2 value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetIntArray(const std::string& name, const int* values, uint32_t count) = 0;
		virtual void SetMatrix4(const std::string& name, const glm::mat4& matrix) = 0;
	public:
		static Ref<Shader> Create(const std::filesystem::path& path);
	};
}