#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/ModuleEntryPoint.h"
#include "GrappleScriptingCore/ScriptingType.h"

Grapple_API void OnModuleLoaded(Grapple::ModuleConfiguration& config)
{
	config.RegisteredTypes = &Grapple::ScriptingType::GetRegisteredTypes();
}

Grapple_API void OnModuleUnloaded(Grapple::ModuleConfiguration& config)
{

}