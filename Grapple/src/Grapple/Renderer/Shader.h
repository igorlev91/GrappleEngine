#pragma once

#include <Grapple/Core/Core.h>

#include <filesystem>

namespace Grapple
{
	class Shader
	{
	public:
		virtual void Bind() = 0;
	public:
		static Ref<Shader> Create(const std::filesystem::path& path);
	};
}