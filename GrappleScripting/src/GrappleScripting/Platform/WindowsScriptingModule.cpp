#include "Grapple/Core/Core.h"

#ifdef Grapple_PLATFORM_WINDOWS

#include "WindowsScriptingModule.h"

#include "Grapple/Core/Log.h"
#include "Grapple/Core/Assert.h"

#include <string>

namespace Grapple
{
	WindowsScriptingModule::WindowsScriptingModule(const std::filesystem::path& path)
		: m_Module(nullptr), m_Loaded(false)
	{
		m_Module = LoadLibraryW(path.c_str());

		DWORD errorCode = GetLastError();
		if (errorCode != 0)
		{
			LPSTR messageBuffer = nullptr;

			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);
			Grapple_CORE_ERROR("Failed to load scripting module: {0}\nError: {1}", path.generic_string(), message);

			return;
		}
		else
			m_Loaded = true;
	}

	WindowsScriptingModule::~WindowsScriptingModule()
	{
		if (m_Loaded)
		{
			FreeLibrary(m_Module);
			m_Loaded = false;
			m_Module = nullptr;
		}
	}

	bool WindowsScriptingModule::IsLoaded()
	{
		return m_Loaded;
	}

	std::optional<ScriptingModuleFunction> WindowsScriptingModule::LoadFunction(const std::string& name)
	{
		Grapple_CORE_ASSERT(m_Loaded);

		auto function = GetProcAddress(m_Module, name.c_str());
		
		DWORD errorCode = GetLastError();
		if (errorCode != 0)
		{
			LPSTR messageBuffer = nullptr;

			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);
			Grapple_CORE_ERROR("Failed to load function {0}\nError: {1}", name, message);
			return {};
		}
		else
		{
			return (ScriptingModuleFunction)function;
		}
	}
}

#endif