#pragma once

#include "Grapple/Math/Math.h"

#include "GrappleECS/World.h"
#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemInitializer.h"

namespace Grapple
{
	class AABBVisualizer : public System
	{
	public:
		Grapple_SYSTEM;

		void OnConfig(World& world, SystemConfig& config) override;
		void OnUpdate(World& world, SystemExecutionContext& context) override;
	private:
		Query m_Query;
		Query m_DecalsQuery;
	};
}