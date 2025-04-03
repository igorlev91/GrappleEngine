#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Platform/Platform.h"

#include <filesystem>
#include <optional>

namespace Grapple
{
	using ScriptingModuleFunction = void(*)();

	class ScriptingModule
	{
	public:
		ScriptingModule();
		ScriptingModule(const std::filesystem::path& path);
		ScriptingModule(const ScriptingModule&) = delete;
		ScriptingModule(ScriptingModule&& other);
		~ScriptingModule();

		void Load(const std::filesystem::path& path);
		bool IsLoaded() const { return m_Library != nullptr; }

		template<typename T>
		std::optional<T> LoadFunction(const std::string& name) const
		{
			std::optional<void*> function = Platform::LoadFunction(m_Library, name);
			if (function.has_value())
				return (T)function.value();
			return {};
		}
	private:
		void* m_Library;
	};
}