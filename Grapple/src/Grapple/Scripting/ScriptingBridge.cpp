#include "ScriptingBridge.h"

namespace Grapple
{
    World* ScriptingBridge::s_World = nullptr;

    static Entity CreateEntity_Wrapper()
    {
        Grapple_CORE_INFO("Creating a new entity from a scripting module :)");
        //return s_World->GetRegistry().CreateEntity(ComponentSet(ids, count));
        return Entity();
    }

    void ScriptingBridge::ConfigureModule(ModuleConfiguration& config)
    {
        using namespace Bindings;

        WorldBindings& worldBinding = *config.WorldBindings;
        worldBinding.CreateEntity = (WorldBindings::CreateEntityFunction)CreateEntity_Wrapper;
    }
}
