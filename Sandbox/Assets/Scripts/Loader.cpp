#include "Grapple/Core/Log.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/ScriptingType.h"
#include "GrappleScriptingCore/SystemInfo.h"
#include "GrappleScriptingCore/ComponentInfo.h"
#include "GrappleScriptingCore/Bindings/ECS/EntityView.h"

Grapple_API void OnModuleLoaded(Grapple::Internal::ModuleConfiguration& config)
{
	Grapple::Log::Initialize();

	config.RegisteredTypes = &Grapple::Internal::ScriptingType::GetRegisteredTypes();
	config.RegisteredSystems = &Grapple::Internal::SystemInfo::GetRegisteredSystems();
	config.RegisteredComponents = &Grapple::Internal::ComponentInfo::GetRegisteredComponents();

	config.WorldBindings = &Grapple::Internal::WorldBindings::Bindings;
	config.EntityViewBindings = &Grapple::Internal::EntityViewBindings::Bindings;
	config.TimeData = &Grapple::Internal::TimeData::Data;
}

Grapple_API void OnModuleUnloaded(Grapple::Internal::ModuleConfiguration& config)
{
}