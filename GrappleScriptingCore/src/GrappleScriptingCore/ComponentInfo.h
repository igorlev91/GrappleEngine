#pragma once

#include "GrappleScriptingCore/Bindings/ECS/Component.h"

#include <string_view>
#include <optional>
#include <vector>

namespace Grapple::Internal
{
	struct ComponentInfo
	{
	public:
		ComponentInfo(std::string_view name)
			: Id(INVALID_COMPONENT_ID), Name(name), AliasedName({})
		{
			GetRegisteredComponents().push_back(this);
		}

		ComponentInfo(std::string_view name, std::string_view aliasedName)
			: Id(INVALID_COMPONENT_ID), Name(name), AliasedName(aliasedName)
		{
			GetRegisteredComponents().push_back(this);
		}
	public:
		static std::vector<ComponentInfo*>& GetRegisteredComponents();
	public:
		ComponentId Id;
		const std::string_view Name;
		const std::optional<std::string_view> AliasedName;
	};
}

#ifndef Grapple_SCRIPTING_CORE_NO_MACROS
	#define Grapple_COMPONENT(component) \
		Grapple_DEFINE_SCRIPTING_TYPE(component) \
		static Grapple::Internal::ComponentInfo Info;
	#define Grapple_COMPONENT_IMPL(component) \
		Grapple_IMPL_SCRIPTING_TYPE(component) \
		Grapple::Internal::ComponentInfo component::Info = Grapple::Internal::ComponentInfo(typeid(component).name());

	#define Grapple_COMPONENT_ALIAS_IMPL(component, aliasedComponent) \
		Grapple_IMPL_SCRIPTING_TYPE(component) \
		Grapple::Internal::ComponentInfo component::Info = Grapple::Internal::ComponentInfo(typeid(component).name(), aliasedComponent);
#endif
