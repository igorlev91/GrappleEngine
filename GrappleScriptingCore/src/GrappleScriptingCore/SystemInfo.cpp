#include "SystemInfo.h"

namespace Grapple::Internal
{
    std::vector<const SystemInfo*>& SystemInfo::GetRegisteredSystems()
    {
        static std::vector<const SystemInfo*> s_RegisteredSystems;
        return s_RegisteredSystems;
    }
}
