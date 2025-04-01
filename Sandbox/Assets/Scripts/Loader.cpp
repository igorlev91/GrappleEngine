#include "Grapple/Core/Log.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemInfo.h"

Grapple_API void OnModuleLoaded(Grapple::ModuleConfiguration& config)
{
	Grapple::Log::Initialize();

	config.RegisteredTypes = &Grapple::ScriptingType::GetRegisteredTypes();
	config.RegisteredSystems = &Grapple::SystemInfo::GetRegisteredSystems();

	config.WorldBindings = &Grapple::Bindings::WorldBindings::Bindings;
}

Grapple_API void OnModuleUnloaded(Grapple::ModuleConfiguration& config)
{
}