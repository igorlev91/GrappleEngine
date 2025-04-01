#include "ScriptingModule.h"

#include "GrappleScripting/Platform/WindowsScriptingModule.h"

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
