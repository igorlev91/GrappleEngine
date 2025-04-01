#pragma once

#include "Grapple/Core/Core.h"

#ifdef Grapple_PLATFORM_WINDOWS

#include "GrappleScripting/ScriptingModule.h"

#include <windows.h>

namespace Grapple
{
	class WindowsScriptingModule : public ScriptingModule
	{
	public:
		WindowsScriptingModule(const std::filesystem::path& path);
		virtual ~WindowsScriptingModule();

		virtual bool IsLoaded() override;

		virtual std::optional<ScriptingModuleFunction> LoadFunction(const std::string& name) override;
	private:
		HMODULE m_Module;
		bool m_Loaded;
	};
}

#endif