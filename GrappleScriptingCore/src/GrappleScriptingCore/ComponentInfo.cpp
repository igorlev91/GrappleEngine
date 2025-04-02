#include "ComponentInfo.h"

namespace Grapple::Internal
{
    std::vector<ComponentInfo*>& ComponentInfo::GetRegisteredComponents()
    {
        static std::vector<ComponentInfo*> s_RegisteredComponents;
        return s_RegisteredComponents;
    }
}
