#pragma once

#include "Grapple/Core/Core.h"
#include "GrapplePlatform/Platform.h"

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
		ScriptingModule(ScriptingModule&& other) noexcept;
		~ScriptingModule();

		void Load(const std::filesystem::path& path);
		bool IsLoaded() const { return m_Library != nullptr; }

		template<typename T>
		std::optional<T> LoadFunction(const std::string& name) const
		{
			void* function = Platform::LoadFunction(m_Library, name);
			if (function != nullptr)
				return (T)function;
			return {};
		}
	private:
		void* m_Library;
	};
}