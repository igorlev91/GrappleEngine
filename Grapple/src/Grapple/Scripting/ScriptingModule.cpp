#include "ScriptingModule.h"

#include "Grapple/Platform/Windows/WindowsScriptingModule.h"

namespace Grapple
{
    Ref<ScriptingModule> Grapple::ScriptingModule::Create(const std::filesystem::path& path)
    {
#ifdef Grapple_PLATFORM_WINDOWS
        return CreateRef<WindowsScriptingModule>(path);
#endif
        return nullptr;
    }
}
