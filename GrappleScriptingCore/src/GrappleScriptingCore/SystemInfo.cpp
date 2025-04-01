#include "SystemInfo.h"

namespace Grapple
{
    std::vector<const SystemInfo*>& Grapple::SystemInfo::GetRegisteredSystems()
    {
        static std::vector<const SystemInfo*> s_RegisteredSystems;
        return s_RegisteredSystems;
    }
}
