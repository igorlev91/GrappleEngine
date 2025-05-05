#include "ShaderCacheManager.h"

namespace Grapple
{
    Scope<ShaderCacheManager> s_Instance;

    void ShaderCacheManager::Uninitialize()
    {
        s_Instance.reset();
    }

    void Grapple::ShaderCacheManager::SetInstance(Scope<ShaderCacheManager>&& cacheManager)
    {
        s_Instance = std::move(cacheManager);
    }

    const Scope<ShaderCacheManager>& Grapple::ShaderCacheManager::GetInstance()
    {
        return s_Instance;
    }
}
