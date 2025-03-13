#pragma once

#include <Grapple/Core/Core.h>

#include <filesystem>

#include <glm/glm.hpp>

namespace Grapple
{
	class Shader
	{
	public:
		virtual void Bind() = 0;

		virtual void SetMatrix4(const std::string& name, const glm::mat4& matrix) = 0;
	public:
		static Ref<Shader> Create(const std::filesystem::path& path);
	};
}