#include "Grapple/Platform/Platform.h"

#include "Grapple/Core/Core.h"
#include "Grapple/Core/Window.h"
#include "Grapple/Core/Application.h"

#include <optional>
#include <filesystem>

#ifdef Grapple_PLATFORM_WINDOWS

#include <windows.h>
#include <commdlg.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Grapple
{
	void* Platform::LoadSharedLibrary(const std::filesystem::path& path)
	{
		HMODULE library = LoadLibraryW(path.c_str());
		DWORD errorCode = GetLastError();
		if (errorCode != 0)
		{
			LPSTR messageBuffer = nullptr;

			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);
			Grapple_CORE_ERROR("Failed to load scripting module: {0}\nError {1}: {2}", path.generic_string(), errorCode, message);

			return nullptr;
		}
		return library;
	}

	void Platform::FreeSharedLibrary(void* library)
	{
		Grapple_CORE_ASSERT(library != nullptr);
		FreeLibrary((HMODULE)library);
	}

	void* Platform::LoadFunction(void* library, const std::string& name)
	{
		Grapple_CORE_ASSERT(library != nullptr);
		auto function = GetProcAddress((HMODULE)library, name.c_str());

		DWORD errorCode = GetLastError();
		if (errorCode != 0)
		{
			LPSTR messageBuffer = nullptr;

			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);
			Grapple_CORE_ERROR("Failed to load function {0}\nError {1}: {2}", name, errorCode, message);
			return nullptr;
		}
		else
			return function;
	}

	std::optional<std::filesystem::path> Platform::ShowOpenFileDialog(const wchar_t* filter)
	{
		Ref<Window> window = Application::GetInstance().GetWindow();

		wchar_t buffer[256] = { 0 };

		OPENFILENAMEW openFile{};
		ZeroMemory(&openFile, sizeof(OPENFILENAMEW));

		openFile.lStructSize = sizeof(OPENFILENAMEW);
		openFile.lpstrFile = buffer;
		openFile.nMaxFile = sizeof(buffer) / sizeof(wchar_t);
		openFile.lpstrFilter = filter;
		openFile.nFilterIndex = 1;
		openFile.nMaxFileTitle = 0;
		openFile.lpstrFileTitle = nullptr;
		openFile.lpstrInitialDir = nullptr;
		openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)window->GetNativeWindow());

		if (GetOpenFileNameW(&openFile) == TRUE)
			return std::filesystem::path(buffer);
		return {};
	}

	std::optional<std::filesystem::path> Platform::ShowSaveFileDialog(const wchar_t* filter)
	{
		Ref<Window> window = Application::GetInstance().GetWindow();

		wchar_t buffer[256] = { 0 };

		OPENFILENAMEW openFile{};
		ZeroMemory(&openFile, sizeof(OPENFILENAMEW));

		openFile.lStructSize = sizeof(OPENFILENAMEW);
		openFile.lpstrFile = buffer;
		openFile.nMaxFile = sizeof(buffer) / sizeof(wchar_t);
		openFile.lpstrFilter = filter;
		openFile.nFilterIndex = 1;
		openFile.nMaxFileTitle = 0;
		openFile.lpstrFileTitle = nullptr;
		openFile.lpstrInitialDir = nullptr;
		openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)window->GetNativeWindow());

		if (GetSaveFileNameW(&openFile) == TRUE)
			return std::filesystem::path(buffer);
		return {};
	}
}
#endif