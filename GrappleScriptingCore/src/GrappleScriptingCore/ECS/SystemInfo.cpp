#include "SystemInfo.h"

namespace Grapple::Scripting
{
    std::vector<SystemInfo*>& SystemInfo::GetRegisteredSystems()
    {
        static std::vector<SystemInfo*> s_RegisteredSystems;
        return s_RegisteredSystems;
    }
}
