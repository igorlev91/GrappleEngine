#include "Grapple/Core/Log.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/ScriptingType.h"

#include "GrappleScriptingCore/ECS/ComponentInfo.h"
#include "GrappleScriptingCore/ECS/SystemInfo.h"
#include "GrappleScriptingCore/ECS/EntityView.h"

Grapple_API void OnModuleLoaded(Grapple::Scripting::ModuleConfiguration& config, Grapple::Scripting::Bindings& bindings)
{
	Grapple::Log::Initialize();

	config.RegisteredTypes = &Grapple::Scripting::ScriptingType::GetRegisteredTypes();
	config.RegisteredSystems = &Grapple::Scripting::SystemInfo::GetRegisteredSystems();
	config.RegisteredComponents = &Grapple::Scripting::ComponentInfo::GetRegisteredComponents();

	config.TimeData = &Grapple::Scripting::TimeData::Data;

	Grapple::Scripting::Bindings::Instance = &bindings;
}

Grapple_API void OnModuleUnloaded(Grapple::Scripting::ModuleConfiguration& config)
{
	Grapple::Scripting::Bindings::Instance = nullptr;
}