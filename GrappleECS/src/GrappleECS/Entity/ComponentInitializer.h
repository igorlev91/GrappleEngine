#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Serialization/TypeInitializer.h"

#include "GrappleECS/Entity/Component.h"

#include <vector>

namespace Grapple
{
	class GrappleECS_API Entities;
	class GrappleECS_API ComponentInitializer
	{
	public:
		ComponentInitializer(const TypeInitializer& type);
		~ComponentInitializer();

		static std::vector<ComponentInitializer*>& GetInitializers();

		constexpr ComponentId GetId() const { return m_Id; }
	public:
		const TypeInitializer& Type;
	private:
		ComponentId m_Id;

		friend class Entities;
	};
}

#define Grapple_COMPONENT                             \
	Grapple_TYPE                                      \
	static Grapple::ComponentInitializer _Component;

#define Grapple_IMPL_COMPONENT(typeName, ...)                               \
	Grapple_IMPL_TYPE(typeName, __VA_ARGS__);                               \
	Grapple::ComponentInitializer typeName::_Component(typeName::_Type);

#define COMPONENT_ID(typeName) (typeName::_Component.GetId())