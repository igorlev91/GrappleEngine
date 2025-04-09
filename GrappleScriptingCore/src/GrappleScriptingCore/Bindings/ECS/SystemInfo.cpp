#include "SystemInfo.h"

namespace Grapple::Internal
{
    std::vector<SystemInfo*>& SystemInfo::GetRegisteredSystems()
    {
        static std::vector<SystemInfo*> s_RegisteredSystems;
        return s_RegisteredSystems;
    }
}
